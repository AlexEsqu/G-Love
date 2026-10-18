/*
 * xml2h4 - convert an XML image into a 4-bit-per-pixel C header for embedded use.
 *
 * Input XML format (row-major pixels, colors as #RGB, #RRGGBB or #RRGGBBAA):
 *
 *   <image width="4" height="2">
 *     <row>#FF0000 #00FF00 #0000FF #FFFFFF</row>
 *     <row>#000000 #808080 #FFFF00 #00FFFF</row>
 *   </image>
 *
 * Only the width/height attributes of <image> and the order of the color
 * tokens after it matter, so any wrapping tags (<row>, <p>, <pixel>...) or
 * whitespace/newline layout works.
 *
 * Output modes:
 *   gray : 16 grayscale levels, no palette
 *   pal  : 16-color palette (median cut), palette emitted as RGB565 uint16_t[16]
 *
 * Packing: 2 pixels per byte, first (left) pixel in the HIGH nibble.
 * Each row is padded to a whole byte (stride = (width + 1) / 2).
 *
 * Build:  gcc -O2 -Wall -o xml2h4 xml2h4.c
 * Usage:  xml2h4 [-m gray|pal] [-n name] [-p] input.xml [output.h]
 *           -p  emit PROGMEM (AVR) so the data stays in flash
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct { uint8_t r, g, b; } rgb_t;

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
    size_t n = fread(buf, 1, (size_t)sz, f);
    fclose(f);
    buf[n] = '\0';
    if (len) *len = n;
    return buf;
}

static int hexval(int c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

/* find name="123" inside [s,e) */
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

/* Parse all #hex colors after the <image ...> tag. Returns pixel count found. */
static size_t parse_pixels(const char *p, rgb_t *px, size_t max)
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
            px[n].r = (uint8_t)(d[0] * 17);
            px[n].g = (uint8_t)(d[1] * 17);
            px[n].b = (uint8_t)(d[2] * 17);
        } else if (k == 6 || k == 8) {
            px[n].r = (uint8_t)(d[0] * 16 + d[1]);
            px[n].g = (uint8_t)(d[2] * 16 + d[3]);
            px[n].b = (uint8_t)(d[4] * 16 + d[5]);
        } else {
            continue;
        }
        n++;
        p += k;
    }
    return n;
}

/* ---- median-cut quantizer to 16 colors ---- */
static int sort_ch;
static int cmp_ch(const void *a, const void *b)
{
    const rgb_t *x = a, *y = b;
    int u = sort_ch == 0 ? x->r : sort_ch == 1 ? x->g : x->b;
    int v = sort_ch == 0 ? y->r : sort_ch == 1 ? y->g : y->b;
    return u - v;
}

static void build_palette(const rgb_t *px, size_t n, rgb_t pal[16])
{
    rgb_t *w = malloc(n * sizeof *w);
    struct { size_t s, e; } box[16];
    int nb = 1;
    memcpy(w, px, n * sizeof *w);
    box[0].s = 0; box[0].e = n;

    while (nb < 16) {
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
            for (int k = 0; k < 3; k++) {
                if (mx[k] - mn[k] > best_range) {
                    best_range = mx[k] - mn[k];
                    best = i;
                    best_ch = k;
                }
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

    for (int i = 0; i < 16; i++) {
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
            pal[i] = pal[nb - 1]; /* unused slots duplicate the last color */
        }
    }
    free(w);
}

static uint8_t nearest(const rgb_t *pal, rgb_t c)
{
    int best = 0;
    long bd = 1L << 30;
    for (int i = 0; i < 16; i++) {
        long dr = (long)c.r - pal[i].r;
        long dg = (long)c.g - pal[i].g;
        long db = (long)c.b - pal[i].b;
        long d = dr * dr + dg * dg + db * db;
        if (d < bd) { bd = d; best = i; }
    }
    return (uint8_t)best;
}

static uint16_t rgb565(rgb_t c)
{
    return (uint16_t)(((c.r & 0xF8) << 8) | ((c.g & 0xFC) << 3) | (c.b >> 3));
}

static void sanitize(char *s)
{
    for (; *s; s++)
        if (!isalnum((unsigned char)*s)) *s = '_';
}

static void usage(const char *a0)
{
    fprintf(stderr, "Usage: %s [-m gray|pal] [-n name] [-p] input.xml [output.h]\n"
                    "  -m  gray (16 gray levels) or pal (16-color palette, default)\n"
                    "  -n  C identifier base name (default: image)\n"
                    "  -p  place data in AVR PROGMEM\n", a0);
}

int main(int argc, char **argv)
{
    const char *mode = "pal", *name = "image", *in = NULL, *out = NULL;
    int progmem = 0;

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-m") && i + 1 < argc) mode = argv[++i];
        else if (!strcmp(argv[i], "-n") && i + 1 < argc) name = argv[++i];
        else if (!strcmp(argv[i], "-p")) progmem = 1;
        else if (!in) in = argv[i];
        else if (!out) out = argv[i];
        else { usage(argv[0]); return 1; }
    }
    if (!in || (strcmp(mode, "gray") && strcmp(mode, "pal"))) { usage(argv[0]); return 1; }
    int gray = !strcmp(mode, "gray");

    size_t len;
    char *xml = read_file(in, &len);
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
    rgb_t *px = malloc(total * sizeof *px);
    size_t got = parse_pixels(tag_end + 1, px, total);
    if (got != total) {
        fprintf(stderr, "Expected %zu pixels (%dx%d) but found %zu\n", total, w, h, got);
        return 1;
    }

    rgb_t pal[16];
    uint8_t *idx = malloc(total);
    if (gray) {
        for (size_t i = 0; i < total; i++) {
            int g = (77 * px[i].r + 150 * px[i].g + 29 * px[i].b) >> 8;
            idx[i] = (uint8_t)((g * 15 + 127) / 255);
        }
    } else {
        build_palette(px, total, pal);
        for (size_t i = 0; i < total; i++) idx[i] = nearest(pal, px[i]);
    }

    size_t stride = ((size_t)w + 1) / 2;
    size_t nbytes = stride * (size_t)h;
    uint8_t *packed = calloc(nbytes, 1);
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            uint8_t v = idx[(size_t)y * w + x] & 0x0F;
            size_t b = (size_t)y * stride + (size_t)x / 2;
            if (x & 1) packed[b] |= v;
            else       packed[b] |= (uint8_t)(v << 4);
        }

    char id[128], up[128];
    snprintf(id, sizeof id, "%s", name);
    sanitize(id);
    for (size_t i = 0; id[i] && i < sizeof up - 1; i++) up[i] = (char)toupper((unsigned char)id[i]), up[i + 1] = '\0';

    FILE *f = out ? fopen(out, "w") : stdout;
    if (!f) { fprintf(stderr, "Cannot write %s\n", out); return 1; }

    fprintf(f, "/* Generated by xml2h4 - %dx%d, 4 bpp (%s), %zu bytes\n"
               " * 2 pixels/byte, left pixel in high nibble, rows padded to whole bytes. */\n",
            w, h, gray ? "16-level grayscale" : "16-color palette", nbytes);
    fprintf(f, "#ifndef %s_H\n#define %s_H\n\n#include <stdint.h>\n", up, up);
    if (progmem) fprintf(f, "#include <avr/pgmspace.h>\n");
    fprintf(f, "\n#define %s_WIDTH  %d\n#define %s_HEIGHT %d\n#define %s_STRIDE %zu\n\n",
            up, w, up, h, up, stride);

    const char *attr = progmem ? " PROGMEM" : "";

    if (!gray) {
        fprintf(f, "/* RGB565 palette */\nstatic const uint16_t %s_palette[16]%s = {\n    ", id, attr);
        for (int i = 0; i < 16; i++)
            fprintf(f, "0x%04X%s%s", rgb565(pal[i]), i < 15 ? "," : "", (i == 15) ? "" : (i % 8 == 7) ? "\n    " : " ");
        fprintf(f, "\n};\n\n");
    }

    fprintf(f, "static const uint8_t %s_data[%zu]%s = {\n    ", id, nbytes, attr);
    for (size_t i = 0; i < nbytes; i++) {
        fprintf(f, "0x%02X", packed[i]);
        if (i + 1 < nbytes) fputs((i % 16 == 15) ? ",\n    " : ", ", f);
    }
    fprintf(f, "\n};\n\n#endif /* %s_H */\n", up);

    if (out) fclose(f);
    free(xml); free(px); free(idx); free(packed);
    return 0;
}
