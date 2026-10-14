/*
 * png2h4 - convert a PNG into a 4-bit-per-pixel C header (one nibble per pixel)
 *          with a 16-entry palette: index 0 = transparent, indices 1..15 = colors.
 *
 * Self-contained: includes its own PNG/zlib decoder (no libpng needed).
 * Supported PNGs: all color types (gray, RGB, palette, gray+alpha, RGBA),
 * bit depths 1/2/4/8/16, tRNS chunks. Interlaced (Adam7) PNGs are rejected.
 *
 * Transparency: pixel is transparent when alpha < threshold (default 128),
 * or when it matches the -t color key.
 *
 * Palette: if the image has <= 15 distinct opaque colors they are kept exactly;
 * otherwise the image is quantized to 15 colors with median cut. Colors are
 * sorted dark -> light into indices 1..15.
 *
 * Packing: 2 pixels per byte, left pixel in the HIGH nibble. Each row is padded
 * to a whole byte (stride = (width + 1) / 2).
 *
 * Output: a shared image4_t typedef (guarded by IMAGE4_T_DEFINED, so several
 * generated headers can be included together) and one instance per image:
 *     typedef struct {
 *         uint16_t width, height;
 *         uint32_t total_pixels;
 *         uint16_t palette[16];      RGB565, [0] = transparent slot
 *         const uint8_t *pixels;     packed 4 bpp pixel map
 *     } image4_t;
 *
 * Build:  gcc -O2 -Wall -o png2h4 png2h4.c
 * Usage:  png2h4 [-n name] [-p] [-a alpha_threshold] [-t RRGGBB] in.png [out.h]
 *           -p  place data in AVR PROGMEM
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { uint8_t r, g, b, a; } rgba_t;
typedef struct { uint8_t r, g, b; } rgb_t;

#define NCOLORS 15 /* index 0 is transparent */

/* =====================  inflate (puff-style)  ===================== */
#define MAXBITS 15
#define MAXLCODES 286
#define MAXDCODES 30
#define FIXLCODES 288

typedef struct {
    const uint8_t *in; size_t inlen, inpos;
    long bitbuf; int bitcnt; int err;
    uint8_t *out; size_t outcap, outpos;
} inf_t;

struct huff { short count[MAXBITS + 1]; short symbol[FIXLCODES]; };

static int bits(inf_t *s, int need)
{
    long val = s->bitbuf;
    while (s->bitcnt < need) {
        if (s->inpos >= s->inlen) { s->err = 1; return 0; }
        val |= (long)s->in[s->inpos++] << s->bitcnt;
        s->bitcnt += 8;
    }
    s->bitbuf = val >> need;
    s->bitcnt -= need;
    return (int)(val & ((1L << need) - 1));
}

static int decode(inf_t *s, const struct huff *h)
{
    int len, code = 0, first = 0, count, index = 0;
    for (len = 1; len <= MAXBITS; len++) {
        code |= bits(s, 1);
        if (s->err) return -1;
        count = h->count[len];
        if (code - count < first) return h->symbol[index + (code - first)];
        index += count; first += count; first <<= 1; code <<= 1;
    }
    return -1;
}

static int construct(struct huff *h, const short *length, int n)
{
    int symbol, len, left;
    short offs[MAXBITS + 1];
    for (len = 0; len <= MAXBITS; len++) h->count[len] = 0;
    for (symbol = 0; symbol < n; symbol++) h->count[length[symbol]]++;
    if (h->count[0] == n) return 0;
    left = 1;
    for (len = 1; len <= MAXBITS; len++) {
        left <<= 1;
        left -= h->count[len];
        if (left < 0) return left;
    }
    offs[1] = 0;
    for (len = 1; len < MAXBITS; len++) offs[len + 1] = (short)(offs[len] + h->count[len]);
    for (symbol = 0; symbol < n; symbol++)
        if (length[symbol] != 0) h->symbol[offs[length[symbol]]++] = (short)symbol;
    return left;
}

static int codes(inf_t *s, const struct huff *lencode, const struct huff *distcode)
{
    static const short lens[29] = {3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,131,163,195,227,258};
    static const short lext[29] = {0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0};
    static const short dists[30] = {1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,1537,2049,3073,4097,6145,8193,12289,16385,24577};
    static const short dext[30] = {0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13};
    int symbol, len;
    unsigned dist;

    do {
        symbol = decode(s, lencode);
        if (symbol < 0 || s->err) return -1;
        if (symbol < 256) {
            if (s->outpos >= s->outcap) return -2;
            s->out[s->outpos++] = (uint8_t)symbol;
        } else if (symbol > 256) {
            symbol -= 257;
            if (symbol >= 29) return -3;
            len = lens[symbol] + bits(s, lext[symbol]);
            symbol = decode(s, distcode);
            if (symbol < 0 || s->err) return -1;
            dist = (unsigned)(dists[symbol] + bits(s, dext[symbol]));
            if (s->err || dist > s->outpos) return -4;
            if (s->outpos + (size_t)len > s->outcap) return -2;
            while (len--) { s->out[s->outpos] = s->out[s->outpos - dist]; s->outpos++; }
        }
    } while (symbol != 256);
    return 0;
}

static int inflate_block_loop(inf_t *s)
{
    int last, type, err;
    struct huff lencode, distcode;
    short lengths[MAXLCODES + MAXDCODES];
    static struct huff fixlen, fixdist;
    static int fixed_built = 0;

    if (!fixed_built) {
        short fl[FIXLCODES];
        int sym;
        for (sym = 0; sym < 144; sym++) fl[sym] = 8;
        for (; sym < 256; sym++) fl[sym] = 9;
        for (; sym < 280; sym++) fl[sym] = 7;
        for (; sym < FIXLCODES; sym++) fl[sym] = 8;
        construct(&fixlen, fl, FIXLCODES);
        for (sym = 0; sym < MAXDCODES; sym++) fl[sym] = 5;
        construct(&fixdist, fl, MAXDCODES);
        fixed_built = 1;
    }

    do {
        last = bits(s, 1);
        type = bits(s, 2);
        if (s->err) return -1;
        if (type == 0) {
            unsigned len, nlen;
            s->bitbuf = 0; s->bitcnt = 0;
            if (s->inpos + 4 > s->inlen) return -1;
            len = s->in[s->inpos] | (unsigned)(s->in[s->inpos + 1] << 8);
            nlen = s->in[s->inpos + 2] | (unsigned)(s->in[s->inpos + 3] << 8);
            s->inpos += 4;
            if (len != (~nlen & 0xFFFF)) return -5;
            if (s->inpos + len > s->inlen) return -1;
            if (s->outpos + len > s->outcap) return -2;
            memcpy(s->out + s->outpos, s->in + s->inpos, len);
            s->inpos += len; s->outpos += len;
        } else if (type == 1) {
            err = codes(s, &fixlen, &fixdist);
            if (err) return err;
        } else if (type == 2) {
            int nlen = bits(s, 5) + 257, ndist = bits(s, 5) + 1, ncode = bits(s, 4) + 4, index;
            static const short order[19] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
            if (s->err || nlen > MAXLCODES || ndist > MAXDCODES) return -6;
            for (index = 0; index < ncode; index++) lengths[order[index]] = (short)bits(s, 3);
            for (; index < 19; index++) lengths[order[index]] = 0;
            err = construct(&lencode, lengths, 19);
            if (err != 0) return -7;
            index = 0;
            while (index < nlen + ndist) {
                int symbol = decode(s, &lencode), len;
                if (symbol < 0 || s->err) return -1;
                if (symbol < 16) lengths[index++] = (short)symbol;
                else {
                    len = 0;
                    if (symbol == 16) {
                        if (index == 0) return -8;
                        len = lengths[index - 1];
                        symbol = 3 + bits(s, 2);
                    } else if (symbol == 17) symbol = 3 + bits(s, 3);
                    else symbol = 11 + bits(s, 7);
                    if (index + symbol > nlen + ndist) return -9;
                    while (symbol--) lengths[index++] = (short)len;
                }
            }
            if (lengths[256] == 0) return -10;
            err = construct(&lencode, lengths, nlen);
            if (err && (err < 0 || nlen != lencode.count[0] + lencode.count[1])) return -11;
            err = construct(&distcode, lengths + nlen, ndist);
            if (err && (err < 0 || ndist != distcode.count[0] + distcode.count[1])) return -12;
            err = codes(s, &lencode, &distcode);
            if (err) return err;
        } else return -13;
    } while (!last);
    return 0;
}

/* zlib stream -> out (exactly outcap bytes expected) */
static int zlib_inflate(const uint8_t *in, size_t inlen, uint8_t *out, size_t outcap)
{
    inf_t s;
    if (inlen < 2 || (in[0] & 0x0F) != 8 || (in[1] & 0x20)) return -20;
    memset(&s, 0, sizeof s);
    s.in = in + 2; s.inlen = inlen - 2;
    s.out = out; s.outcap = outcap;
    int r = inflate_block_loop(&s);
    if (r) return r;
    return s.outpos == outcap ? 0 : -21;
}

/* =====================  PNG decoder  ===================== */
static uint32_t be32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }
static unsigned be16(const uint8_t *p) { return (unsigned)(p[0] << 8 | p[1]); }

static char *read_file(const char *path, size_t *len)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) { fclose(f); return NULL; }
    char *buf = malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    *len = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    return buf;
}

static unsigned get_sample(const uint8_t *row, size_t i, int depth)
{
    if (depth == 8) return row[i];
    if (depth == 16) return (unsigned)(row[2 * i] << 8 | row[2 * i + 1]);
    size_t bitpos = i * (size_t)depth;
    int shift = 8 - depth - (int)(bitpos & 7);
    return (row[bitpos >> 3] >> shift) & ((1u << depth) - 1);
}

static uint8_t scale8(unsigned v, int depth)
{
    if (depth == 8) return (uint8_t)v;
    if (depth == 16) return (uint8_t)(v >> 8);
    return (uint8_t)(v * 255u / ((1u << depth) - 1));
}

static int paeth(int a, int b, int c)
{
    int p = a + b - c, pa = abs(p - a), pb = abs(p - b), pc = abs(p - c);
    return (pa <= pb && pa <= pc) ? a : (pb <= pc) ? b : c;
}

/* returns NULL on success, otherwise an error message */
static const char *png_load(const char *path, int *ow, int *oh, rgba_t **opx)
{
    static const uint8_t sig[8] = {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A};
    size_t len;
    uint8_t *d = (uint8_t *)read_file(path, &len);
    if (!d) return "cannot read file";
    if (len < 8 || memcmp(d, sig, 8)) return "not a PNG file";

    uint32_t w = 0, h = 0;
    int depth = 0, ctype = -1, have_ihdr = 0;
    uint8_t plte[256][3];
    uint8_t trns_pal[256];
    int nplte = 0, ntrns = 0, have_key = 0;
    unsigned key[3] = {0, 0, 0};
    uint8_t *idat = NULL;
    size_t idat_len = 0;
    memset(trns_pal, 255, sizeof trns_pal);

    size_t pos = 8;
    while (len - pos >= 12) {
        uint32_t clen = be32(d + pos);
        const uint8_t *type = d + pos + 4, *cd = d + pos + 8;
        if (clen > len - pos - 12) return "truncated chunk";
        if (!memcmp(type, "IHDR", 4)) {
            if (clen < 13) return "bad IHDR";
            w = be32(cd); h = be32(cd + 4);
            depth = cd[8]; ctype = cd[9];
            if (cd[10] != 0 || cd[11] != 0) return "unsupported compression/filter method";
            if (cd[12] != 0) return "interlaced PNG not supported (re-save without interlacing)";
            have_ihdr = 1;
        } else if (!memcmp(type, "PLTE", 4)) {
            nplte = (int)(clen / 3);
            if (nplte > 256) nplte = 256;
            for (int i = 0; i < nplte; i++) memcpy(plte[i], cd + 3 * i, 3);
        } else if (!memcmp(type, "tRNS", 4)) {
            if (!have_ihdr) return "tRNS before IHDR";
            if (ctype == 3) {
                ntrns = clen > 256 ? 256 : (int)clen;
                memcpy(trns_pal, cd, (size_t)ntrns);
            } else if (ctype == 0 && clen >= 2) {
                key[0] = be16(cd); have_key = 1;
            } else if (ctype == 2 && clen >= 6) {
                key[0] = be16(cd); key[1] = be16(cd + 2); key[2] = be16(cd + 4); have_key = 1;
            }
        } else if (!memcmp(type, "IDAT", 4)) {
            uint8_t *n = realloc(idat, idat_len + clen);
            if (!n) return "out of memory";
            idat = n;
            memcpy(idat + idat_len, cd, clen);
            idat_len += clen;
        } else if (!memcmp(type, "IEND", 4)) {
            break;
        }
        pos += 12 + (size_t)clen;
    }
    if (!have_ihdr || !idat) return "missing IHDR or IDAT";
    if (w == 0 || h == 0 || w > 16384 || h > 16384) return "unsupported image size";

    int channels;
    switch (ctype) {
        case 0: channels = 1; if (depth != 1 && depth != 2 && depth != 4 && depth != 8 && depth != 16) return "bad bit depth"; break;
        case 2: channels = 3; if (depth != 8 && depth != 16) return "bad bit depth"; break;
        case 3: channels = 1; if (depth != 1 && depth != 2 && depth != 4 && depth != 8) return "bad bit depth"; break;
        case 4: channels = 2; if (depth != 8 && depth != 16) return "bad bit depth"; break;
        case 6: channels = 4; if (depth != 8 && depth != 16) return "bad bit depth"; break;
        default: return "bad color type";
    }
    if (ctype == 3 && nplte == 0) return "palette PNG without PLTE";

    size_t bpbits = (size_t)channels * (size_t)depth;
    size_t rb = ((size_t)w * bpbits + 7) / 8;
    size_t bpp = bpbits / 8 ? bpbits / 8 : 1;
    size_t expected = (size_t)h * (rb + 1);
    uint8_t *raw = malloc(expected);
    if (!raw) return "out of memory";
    int r = zlib_inflate(idat, idat_len, raw, expected);
    if (r) { static char msg[64]; snprintf(msg, sizeof msg, "zlib decode error (%d)", r); return msg; }

    /* unfilter in place */
    for (size_t y = 0; y < h; y++) {
        uint8_t *row = raw + y * (rb + 1);
        int f = row[0];
        uint8_t *cur = row + 1;
        const uint8_t *prev = y ? raw + (y - 1) * (rb + 1) + 1 : NULL;
        if (f > 4) return "bad PNG filter type";
        for (size_t i = 0; i < rb; i++) {
            int a = i >= bpp ? cur[i - bpp] : 0;
            int b = prev ? prev[i] : 0;
            int c = (prev && i >= bpp) ? prev[i - bpp] : 0;
            int add = 0;
            switch (f) {
                case 1: add = a; break;
                case 2: add = b; break;
                case 3: add = (a + b) >> 1; break;
                case 4: add = paeth(a, b, c); break;
            }
            cur[i] = (uint8_t)(cur[i] + add);
        }
    }

    rgba_t *px = malloc((size_t)w * h * sizeof *px);
    if (!px) return "out of memory";
    for (size_t y = 0; y < h; y++) {
        const uint8_t *row = raw + y * (rb + 1) + 1;
        for (size_t x = 0; x < w; x++) {
            rgba_t *o = &px[y * w + x];
            o->a = 255;
            if (ctype == 3) {
                unsigned i = get_sample(row, x, depth);
                if ((int)i >= nplte) return "palette index out of range";
                o->r = plte[i][0]; o->g = plte[i][1]; o->b = plte[i][2];
                o->a = trns_pal[i];
            } else if (ctype == 0 || ctype == 4) {
                unsigned g = get_sample(row, x * (size_t)channels, depth);
                o->r = o->g = o->b = scale8(g, depth);
                if (ctype == 4) o->a = scale8(get_sample(row, x * 2 + 1, depth), depth);
                else if (have_key && g == key[0]) o->a = 0;
            } else {
                unsigned v[4] = {0, 0, 0, 255};
                for (int c = 0; c < channels; c++) v[c] = get_sample(row, x * (size_t)channels + (size_t)c, depth);
                o->r = scale8(v[0], depth); o->g = scale8(v[1], depth); o->b = scale8(v[2], depth);
                if (ctype == 6) o->a = scale8(v[3], depth);
                else if (have_key && v[0] == key[0] && v[1] == key[1] && v[2] == key[2]) o->a = 0;
            }
        }
    }
    free(raw); free(idat); free(d);
    *ow = (int)w; *oh = (int)h; *opx = px;
    return NULL;
}

/* =====================  quantizer  ===================== */
static int sort_ch;
static int cmp_ch(const void *a, const void *b)
{
    const rgb_t *x = a, *y = b;
    int u = sort_ch == 0 ? x->r : sort_ch == 1 ? x->g : x->b;
    int v = sort_ch == 0 ? y->r : sort_ch == 1 ? y->g : y->b;
    return u - v;
}

static void build_palette(const rgb_t *px, size_t n, rgb_t *pal, int ncol)
{
    rgb_t *w = malloc(n * sizeof *w);
    struct { size_t s, e; } box[16];
    int nb = 1;
    memcpy(w, px, n * sizeof *w);
    box[0].s = 0; box[0].e = n;

    while (nb < ncol) {
        int best = -1, best_range = -1, best_ch = 0;
        for (int i = 0; i < nb; i++) {
            if (box[i].e - box[i].s < 2) continue;
            int mn[3] = {255, 255, 255}, mx[3] = {0, 0, 0};
            for (size_t j = box[i].s; j < box[i].e; j++) {
                int c[3] = {w[j].r, w[j].g, w[j].b};
                for (int k = 0; k < 3; k++) {
                    if (c[k] < mn[k]) mn[k] = c[k];
                    if (c[k] > mx[k]) mx[k] = c[k];
                }
            }
            for (int k = 0; k < 3; k++)
                if (mx[k] - mn[k] > best_range) { best_range = mx[k] - mn[k]; best = i; best_ch = k; }
        }
        if (best < 0 || best_range <= 0) break;
        sort_ch = best_ch;
        qsort(w + box[best].s, box[best].e - box[best].s, sizeof *w, cmp_ch);
        size_t mid = (box[best].s + box[best].e) / 2;
        box[nb].s = mid; box[nb].e = box[best].e; box[best].e = mid;
        nb++;
    }
    for (int i = 0; i < ncol; i++) {
        if (i < nb) {
            unsigned long r = 0, g = 0, b = 0;
            size_t cnt = box[i].e - box[i].s;
            for (size_t j = box[i].s; j < box[i].e; j++) { r += w[j].r; g += w[j].g; b += w[j].b; }
            pal[i].r = (uint8_t)(r / cnt); pal[i].g = (uint8_t)(g / cnt); pal[i].b = (uint8_t)(b / cnt);
        } else {
            pal[i] = pal[nb - 1];
        }
    }
    free(w);
}

static int luma(rgb_t c) { return 77 * c.r + 150 * c.g + 29 * c.b; }
static int cmp_luma(const void *a, const void *b) { return luma(*(const rgb_t *)a) - luma(*(const rgb_t *)b); }

static int nearest(const rgb_t *pal, int n, rgb_t c)
{
    int best = 0;
    long bd = 1L << 30;
    for (int i = 0; i < n; i++) {
        long dr = (long)c.r - pal[i].r, dg = (long)c.g - pal[i].g, db = (long)c.b - pal[i].b;
        long d = dr * dr + dg * dg + db * db;
        if (d < bd) { bd = d; best = i; }
    }
    return best;
}

static uint16_t rgb565(rgb_t c) { return (uint16_t)(((c.r & 0xF8) << 8) | ((c.g & 0xFC) << 3) | (c.b >> 3)); }

static void usage(const char *a0)
{
    fprintf(stderr, "Usage: %s [-n name] [-p] [-a alpha_threshold] [-t RRGGBB] in.png [out.h]\n"
                    "  -n  C identifier base name (default: image)\n"
                    "  -p  place data in AVR PROGMEM\n"
                    "  -a  alpha below this (0-255) is transparent (default 128)\n"
                    "  -t  color key (RRGGBB) treated as transparent\n", a0);
}

int main(int argc, char **argv)
{
    const char *name = "image", *in = NULL, *out = NULL;
    int progmem = 0, athr = 128, have_key = 0;
    rgb_t key = {0, 0, 0};

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n") && i + 1 < argc) name = argv[++i];
        else if (!strcmp(argv[i], "-p")) progmem = 1;
        else if (!strcmp(argv[i], "-a") && i + 1 < argc) athr = atoi(argv[++i]);
        else if (!strcmp(argv[i], "-t") && i + 1 < argc) {
            const char *s = argv[++i];
            unsigned v;
            if (*s == '#') s++;
            if (strlen(s) != 6 || sscanf(s, "%x", &v) != 1) { usage(argv[0]); return 1; }
            key.r = (uint8_t)(v >> 16); key.g = (uint8_t)(v >> 8); key.b = (uint8_t)v;
            have_key = 1;
        }
        else if (!in) in = argv[i];
        else if (!out) out = argv[i];
        else { usage(argv[0]); return 1; }
    }
    if (!in) { usage(argv[0]); return 1; }

    int w, h;
    rgba_t *px;
    const char *err = png_load(in, &w, &h, &px);
    if (err) { fprintf(stderr, "%s: %s\n", in, err); return 1; }
    size_t total = (size_t)w * (size_t)h;

    uint8_t *opaque = malloc(total);
    rgb_t *op = malloc(total * sizeof *op);
    size_t nop = 0;
    for (size_t i = 0; i < total; i++) {
        int t = px[i].a < athr ||
                (have_key && px[i].r == key.r && px[i].g == key.g && px[i].b == key.b);
        opaque[i] = !t;
        if (!t) op[nop++] = (rgb_t){px[i].r, px[i].g, px[i].b};
    }

    /* exact palette if <= 15 distinct opaque colors, otherwise median cut */
    rgb_t pal[NCOLORS + 1];
    int npal = 0, exact = 1;
    for (size_t i = 0; i < nop && exact; i++) {
        int j;
        for (j = 0; j < npal; j++)
            if (pal[j].r == op[i].r && pal[j].g == op[i].g && pal[j].b == op[i].b) break;
        if (j == npal) {
            if (npal < NCOLORS) pal[npal++] = op[i];
            else exact = 0;
        }
    }
    if (!exact) { build_palette(op, nop, pal, NCOLORS); npal = NCOLORS; }
    qsort(pal, (size_t)npal, sizeof pal[0], cmp_luma);

    size_t stride = ((size_t)w + 1) / 2, nbytes = stride * (size_t)h;
    uint8_t *packed = calloc(nbytes, 1);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            size_t i = (size_t)y * (size_t)w + (size_t)x;
            uint8_t v = 0; /* 0 = transparent */
            if (opaque[i]) {
                rgb_t c = {px[i].r, px[i].g, px[i].b};
                v = (uint8_t)(nearest(pal, npal, c) + 1);
            }
            size_t b = (size_t)y * stride + (size_t)x / 2;
            if (x & 1) packed[b] |= v;
            else       packed[b] |= (uint8_t)(v << 4);
        }

    char id[128], up[128];
    snprintf(id, sizeof id, "%s", name);
    for (char *s = id; *s; s++) if (!isalnum((unsigned char)*s)) *s = '_';
    size_t k = 0;
    for (; id[k]; k++) up[k] = (char)toupper((unsigned char)id[k]);
    up[k] = '\0';

    FILE *f = out ? fopen(out, "w") : stdout;
    if (!f) { fprintf(stderr, "Cannot write %s\n", out); return 1; }
    const char *attr = progmem ? " PROGMEM" : "";

    fprintf(f, "/* Generated by png2h4 - %dx%d, 4 bpp, %zu bytes.\n"
               " * Palette index 0 = transparent, 1..15 = colors (%s, %d used).\n"
               " * 2 pixels/byte, left pixel in high nibble, rows padded to whole bytes. */\n",
            w, h, nbytes, exact ? "exact" : "quantized", npal);
    fprintf(f, "#ifndef %s_H\n#define %s_H\n\n#include <stdint.h>\n", up, up);
    if (progmem) fprintf(f, "#include <avr/pgmspace.h>\n");
    fprintf(f, "\n#ifndef IMAGE4_T_DEFINED\n#define IMAGE4_T_DEFINED\n"
               "typedef struct {\n"
               "    uint16_t width;\n"
               "    uint16_t height;\n"
               "    uint32_t total_pixels;      /* width * height */\n"
               "    uint16_t palette[16];       /* RGB565; [0] = transparent slot (unused) */\n"
               "    const uint8_t *pixels;      /* 4 bpp, 2 px/byte, high nibble first, rows padded to bytes */\n"
               "} image4_t;\n#endif\n");
    fprintf(f, "\n#define %s_WIDTH       %d\n#define %s_HEIGHT      %d\n"
               "#define %s_STRIDE      %zu\n#define %s_TRANSPARENT 0\n\n",
            up, w, up, h, up, stride, up);

    fprintf(f, "static const uint8_t %s_data[%zu]%s = {\n    ", id, nbytes, attr);
    for (size_t i = 0; i < nbytes; i++) {
        fprintf(f, "0x%02X", packed[i]);
        if (i + 1 < nbytes) fputs(((i + 1) % stride == 0) ? ",\n    " : ", ", f);
    }
    fprintf(f, "\n};\n\n");

    fprintf(f, "static const image4_t %s%s = {\n"
               "    %s_WIDTH,\n    %s_HEIGHT,\n    %zuUL,\n"
               "    { /* palette */\n        0x0000,",
            id, attr, up, up, total);
    for (int i = 0; i < 15; i++) {
        uint16_t c = i < npal ? rgb565(pal[i]) : 0;
        fprintf(f, " 0x%04X%s", c, i < 14 ? "," : "");
        if (i == 6) fprintf(f, "\n        ");
    }
    fprintf(f, "\n    },\n    %s_data\n};\n\n#endif /* %s_H */\n", id, up);

    if (out) fclose(f);
    fprintf(stderr, "%dx%d -> %zu bytes, %d colors + transparent (%s)\n", w, h, nbytes, npal, exact ? "exact" : "quantized");
    free(px); free(opaque); free(op); free(packed);
    return 0;
}
