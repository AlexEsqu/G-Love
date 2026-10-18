/*
 * xml2h8 - convert an XML image into a 1-byte-per-pixel C header with
 *          7 colors + 1 transparent value, for embedded use.
 *
 * Input XML (row-major pixels; colors as #RGB, #RRGGBB or #RRGGBBAA):
 *
 *   <image width="4" height="2">
 *     <row>#FF0000 #00FF00 #0000FF #FFFFFF</row>
 *     <row>#00000000 #808080 #FFFF00 #00FFFF</row>
 *   </image>
 *
 * Only width/height of <image> and the order of the color tokens matter.
 *
 * Pixel byte values (edit CODES[] below to change them):
 * 
 *   0b0000                      transparent
 *   0b1000 1100 1110 1111 0111 0011 0001   the 7 colors, assigned to the
 *                                          quantized palette from dark to light
 *
 * A pixel is transparent when its alpha < 128 or it equals the -t color key.
 *
 * Build:  gcc -O2 -Wall -o xml2h8 xml2h8.c
 * Usage:  xml2h8 [-n name] [-p] [-H] [-t RRGGBB] input.xml [output.h]
 *           -p  PROGMEM (AVR)       -H  put the code in the HIGH nibble
 *           -t  color key treated as transparent
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NCOLORS 7
static const uint8_t CODES[NCOLORS] = {0x8, 0xC, 0xE, 0xF, 0x7, 0x3, 0x1};
#define TRANSPARENT_CODE 0x0

typedef struct { uint8_t r, g, b, a; } rgba_t;
typedef struct { uint8_t r, g, b; } rgb_t;

static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    if (sz < 0) { fclose(f); return NULL; }
    char *buf = malloc((size_t)sz + 1);
    if (!buf) { fclose(f); return NULL; }
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[n] = '\0';
    return buf;
}

static int hexval(int c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static int get_attr(const char *s, const char *e, const char *name, int *out)
{
    size_t nl = strlen(name);
    for (const char *p = s; p + nl < e; p++) {
        if (strncmp(p, name, nl) == 0 && (p == s || isspace((unsigned char)p[-1]))) {
            const char *q = p + nl;
            while (q < e && isspace((unsigned char)*q)) q++;
            if (q < e && *q == '=') {
                q++;
                while (q < e && isspace((unsigned char)*q)) q++;
                if (q < e && (*q == '"' || *q == '\'')) q++;
                *out = atoi(q);
                return 1;
            }
        }
    }
    return 0;
}

/* Parse #RGB / #RRGGBB / #RRGGBBAA tokens. Returns number of pixels found. */
static size_t parse_pixels(const char *p, rgba_t *px, size_t max)
{
    size_t n = 0;
    for (; *p && n < max; p++) {
        if (*p != '#') continue;
        int d[8], k = 0;
        while (k < 8 && hexval((unsigned char)p[1 + k]) >= 0) {
            d[k] = hexval((unsigned char)p[1 + k]);
            k++;
        }
        if (k == 3) {
            px[n] = (rgba_t){(uint8_t)(d[0] * 17), (uint8_t)(d[1] * 17), (uint8_t)(d[2] * 17), 255};
        } else if (k == 6 || k == 8) {
            px[n].r = (uint8_t)(d[0] * 16 + d[1]);
            px[n].g = (uint8_t)(d[2] * 16 + d[3]);
            px[n].b = (uint8_t)(d[4] * 16 + d[5]);
            px[n].a = (k == 8) ? (uint8_t)(d[6] * 16 + d[7]) : 255;
        } else {
            continue;
        }
        n++;
        p += k;
    }
    return n;
}

/* ---- median-cut quantizer ---- */
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
    memset(pal, 0, (size_t)ncol * sizeof *pal);
    if (n == 0) return;

    rgb_t *w = malloc(n * sizeof *w);
    struct { size_t s, e; } box[NCOLORS];
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
                if (mx[k] - mn[k] > best_range) {
                    best_range = mx[k] - mn[k];
                    best = i;
                    best_ch = k;
                }
        }
        if (best < 0 || best_range <= 0) break;
        sort_ch = best_ch;
        qsort(w + box[best].s, box[best].e - box[best].s, sizeof *w, cmp_ch);
        size_t mid = (box[best].s + box[best].e) / 2;
        box[nb].s = mid;
        box[nb].e = box[best].e;
        box[best].e = mid;
        nb++;
    }

    for (int i = 0; i < ncol; i++) {
        if (i < nb) {
            unsigned long r = 0, g = 0, b = 0;
            size_t cnt = box[i].e - box[i].s;
            for (size_t j = box[i].s; j < box[i].e; j++) {
                r += w[j].r; g += w[j].g; b += w[j].b;
            }
            pal[i].r = (uint8_t)(r / cnt);
            pal[i].g = (uint8_t)(g / cnt);
            pal[i].b = (uint8_t)(b / cnt);
        } else {
            pal[i] = pal[nb - 1];
        }
    }
    free(w);
}

static int luma(rgb_t c) { return 77 * c.r + 150 * c.g + 29 * c.b; }
static int cmp_luma(const void *a, const void *b) { return luma(*(const rgb_t *)a) - luma(*(const rgb_t *)b); }

static int nearest(const rgb_t *pal, int ncol, rgb_t c)
{
    int best = 0;
    long bd = 1L << 30;
    for (int i = 0; i < ncol; i++) {
        long dr = (long)c.r - pal[i].r, dg = (long)c.g - pal[i].g, db = (long)c.b - pal[i].b;
        long d = dr * dr + dg * dg + db * db;
        if (d < bd) { bd = d; best = i; }
    }
    return best;
}

static uint16_t rgb565(rgb_t c)
{
    return (uint16_t)(((c.r & 0xF8) << 8) | ((c.g & 0xFC) << 3) | (c.b >> 3));
}

static void bin4(char *dst, unsigned v)
{
    for (int i = 3; i >= 0; i--) *dst++ = (v >> i & 1) ? '1' : '0';
    *dst = '\0';
}

static void usage(const char *a0)
{
    fprintf(stderr, "Usage: %s [-n name] [-p] [-H] [-t RRGGBB] input.xml [output.h]\n"
                    "  -n  C identifier base name (default: image)\n"
                    "  -p  place data in AVR PROGMEM\n"
                    "  -H  store the 4-bit code in the high nibble (default: low)\n"
                    "  -t  color key (RRGGBB) treated as transparent\n", a0);
}

int main(int argc, char **argv)
{
    const char *name = "image", *in = NULL, *out = NULL;
    int progmem = 0, high = 0, have_key = 0;
    rgb_t key = {0, 0, 0};

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-n") && i + 1 < argc) name = argv[++i];
        else if (!strcmp(argv[i], "-p")) progmem = 1;
        else if (!strcmp(argv[i], "-H")) high = 1;
        else if (!strcmp(argv[i], "-t") && i + 1 < argc) {
            const char *s = argv[++i];
            if (*s == '#') s++;
            unsigned v;
            if (strlen(s) != 6 || sscanf(s, "%x", &v) != 1) { usage(argv[0]); return 1; }
            key.r = (uint8_t)(v >> 16); key.g = (uint8_t)(v >> 8); key.b = (uint8_t)v;
            have_key = 1;
        }
        else if (!in) in = argv[i];
        else if (!out) out = argv[i];
        else { usage(argv[0]); return 1; }
    }
    if (!in) { usage(argv[0]); return 1; }

    char *xml = read_file(in);
    if (!xml) { fprintf(stderr, "Cannot read %s\n", in); return 1; }

    const char *tag = strstr(xml, "<image");
    const char *tag_end = tag ? strchr(tag, '>') : NULL;
    if (!tag || !tag_end) { fprintf(stderr, "No <image ...> tag found\n"); return 1; }

    int w = 0, h = 0;
    if (!get_attr(tag, tag_end, "width", &w) || !get_attr(tag, tag_end, "height", &h) ||
        w <= 0 || h <= 0) {
        fprintf(stderr, "<image> needs positive width and height attributes\n");
        return 1;
    }

    size_t total = (size_t)w * (size_t)h;
    rgba_t *px = malloc(total * sizeof *px);
    size_t got = parse_pixels(tag_end + 1, px, total);
    if (got != total) {
        fprintf(stderr, "Expected %zu pixels (%dx%d) but found %zu\n", total, w, h, got);
        return 1;
    }

    /* classify transparency, gather opaque pixels for quantization */
    uint8_t *opaque = malloc(total);
    rgb_t *op = malloc(total * sizeof *op);
    size_t nop = 0;
    for (size_t i = 0; i < total; i++) {
        int t = px[i].a < 128 ||
                (have_key && px[i].r == key.r && px[i].g == key.g && px[i].b == key.b);
        opaque[i] = !t;
        if (!t) op[nop++] = (rgb_t){px[i].r, px[i].g, px[i].b};
    }

    rgb_t pal[NCOLORS];
    build_palette(op, nop, pal, NCOLORS);
    qsort(pal, NCOLORS, sizeof pal[0], cmp_luma); /* dark -> light */

    int shift = high ? 4 : 0;
    uint8_t *data = malloc(total);
    for (size_t i = 0; i < total; i++) {
        uint8_t code = TRANSPARENT_CODE;
        if (opaque[i]) {
            rgb_t c = {px[i].r, px[i].g, px[i].b};
            code = CODES[nearest(pal, NCOLORS, c)];
        }
        data[i] = (uint8_t)(code << shift);
    }

    /* identifier / macro names */
    char id[128], up[128];
    snprintf(id, sizeof id, "%s", name);
    for (char *s = id; *s; s++) if (!isalnum((unsigned char)*s)) *s = '_';
    size_t k = 0;
    for (; id[k]; k++) up[k] = (char)toupper((unsigned char)id[k]);
    up[k] = '\0';

    FILE *f = out ? fopen(out, "w") : stdout;
    if (!f) { fprintf(stderr, "Cannot write %s\n", out); return 1; }

    const char *attr = progmem ? " PROGMEM" : "";
    fprintf(f, "/* Generated by xml2h8 - %dx%d, 1 byte/pixel, 7 colors + transparent, %zu bytes.\n"
               " * Code is in the %s nibble of each byte; 0 = transparent.\n"
               " * Palette is indexed directly by the 4-bit code (RGB565). */\n",
            w, h, total, high ? "HIGH" : "LOW");
    fprintf(f, "#ifndef %s_H\n#define %s_H\n\n#include <stdint.h>\n", up, up);
    if (progmem) fprintf(f, "#include <avr/pgmspace.h>\n");
    fprintf(f, "\n#define %s_WIDTH       %d\n#define %s_HEIGHT      %d\n"
               "#define %s_NIBBLE_SHIFT %d\n#define %s_TRANSPARENT 0x%02X\n\n",
            up, w, up, h, up, shift, up, TRANSPARENT_CODE);

    /* palette indexed by code */
    uint16_t pal16[16] = {0};
    char b4[8];
    for (int i = 0; i < NCOLORS; i++) pal16[CODES[i]] = rgb565(pal[i]);
    fprintf(f, "/* index = 4-bit code; entry 0 (transparent) and unused codes are 0 */\n"
               "static const uint16_t %s_palette[16]%s = {\n", id, attr);
    for (int i = 0; i < 16; i++) {
        bin4(b4, (unsigned)i);
        fprintf(f, "    0x%04X%s /* 0b%s */\n", pal16[i], i < 15 ? "," : "", b4);
    }
    fprintf(f, "};\n\n");

    fprintf(f, "static const uint8_t %s_data[%zu]%s = {\n    ", id, total, attr);
    for (size_t i = 0; i < total; i++) {
        fprintf(f, "0x%02X", data[i]);
        if (i + 1 < total) fputs(((i + 1) % (size_t)w == 0 || i % 16 == 15) ? ",\n    " : ", ", f);
    }
    fprintf(f, "\n};\n\n#endif /* %s_H */\n", up);

    if (out) fclose(f);
    free(xml); free(px); free(opaque); free(op); free(data);
    return 0;
}
