#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// Standard BMP Header Structs
#pragma pack(push, 1)
typedef struct {
	uint16_t type;
	uint32_t size;
	uint16_t reserved1;
	uint16_t reserved2;
	uint32_t offset;
} BMPHeader;

typedef struct {
	uint32_t size;
	int32_t  width;
	int32_t  height;
	uint16_t planes;
	uint16_t bits_per_pixel;
	uint32_t compression;
	uint32_t image_size;
	int32_t  x_resolution;
	int32_t  y_resolution;
	uint32_t colors_used;
	uint32_t colors_important;
} BMPInfoHeader;
#pragma pack(pop)

#define MAX_BMP_WIDTH 300

int bmp_to_progmem_header(const char* bmp_path, const char* header_path, const char* array_name)
{
	FILE* infile = fopen(bmp_path, "rb");

	if (!infile)
	{
		printf("Error: Cannot open input BMP file %s.\n", bmp_path);
		return -1;
	}

	BMPHeader header;
	BMPInfoHeader info_header;
	fread(&header, sizeof(BMPHeader), 1, infile);
	fread(&info_header, sizeof(BMPInfoHeader), 1, infile);

	int32_t width = info_header.width;
	int32_t height = abs(info_header.height);
	int is_top_down = (info_header.height < 0);
	int chunk = 1;

	FILE* outfile = fopen(header_path, "w");
	if (!outfile)
	{
		printf("Error: Cannot create output header file.\n");
		fclose(infile);
		return -1;
	}

	// Calculate row padding
	int row_stride = width * 3;
	int row_padding = (4 - (row_stride % 4)) % 4;
	int padded_row_stride = row_stride + row_padding;

	fprintf(outfile, "#ifndef IMAGE_%s_H\n", array_name);
	fprintf(outfile, "#define IMAGE_%s_H\n\n", array_name);
	fprintf(outfile, "#include <stdint.h>\n");
	fprintf(outfile, "#include <stdint.h>\n");
	fprintf(outfile, "const uint16_t %s_%d[] PROGMEM  = {\n", array_name, chunk++);

	uint8_t line_buffer[MAX_BMP_WIDTH * 3 + 4];
	int total_pixels_written = 0;

	for (int32_t y = 0; y < height; y++)
	{
		// So why have picture on backward ???
		int32_t bmp_y = is_top_down ? y : (height - 1 - y);
		long row_file_offset = header.offset + (bmp_y * padded_row_stride);

		fseek(infile, row_file_offset, SEEK_SET);
		if (fread(line_buffer, 1, padded_row_stride, infile) != (size_t)padded_row_stride)
		{
			printf("Error reading line data at row %d.\n", y);
			fclose(infile);
			fclose(outfile);
			return -1;
		}

		for (int32_t x = 0; x < width; x++)
		{
			uint16_t pixel_565;

			uint8_t r = line_buffer[x * 3 + 0];
			uint8_t g = line_buffer[x * 3 + 1];
			uint8_t b = line_buffer[x * 3 + 2];

			uint8_t r_5bit = (r >> 3) & 0x1F;
			uint8_t g_6bit = (g >> 2) & 0x3F;
			uint8_t b_5bit = (b >> 3) & 0x1F;
			pixel_565 = (r_5bit << 11) | (g_6bit << 5) | b_5bit;

			// Big Endian because display drivers like to eat their eggs big side first
			uint16_t big_endian_565 = ((pixel_565 & 0xFF) << 8) | ((pixel_565 >> 8) & 0xFF);

			fprintf(outfile, "0x%04X, ", pixel_565);
			total_pixels_written++;

			if (total_pixels_written % 240 == 0)
			{
				fprintf(outfile, "\n");
			}

			if (total_pixels_written % 14400 == 0)
			{
				fprintf(outfile, "};\n\nconst uint16_t %s_%d[] PROGMEM = {\n", array_name, chunk++);
			}
		}
	}

	fprintf(outfile, "\n};\n\n#endif // IMAGE_%s_H\n", array_name);

	fclose(infile);
	fclose(outfile);
	printf("Successfully wrote header array to %s\n", header_path);
	return 0;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("Error: Please give me a .bmp file to open, thanks...\n");
		return -1;
	}

	bmp_to_progmem_header(argv[1], "result.h", "result");
	return 0;
}
