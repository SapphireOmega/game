#include "imgload.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *img_strerror(enum img_err img_err)
{
	char *err = (char *)malloc(sizeof(char) * 256);

	switch (img_err) {
	case IMG_ERR_NONE:
		strcpy(err, "no error");
		break;
	case IMG_ERR_OPEN:
		strcpy(err, "error opening image file");
		break;
	case IMG_ERR_TYPE:
		strcpy(err, "unsupported tga type, only supports uncompressed");
		break;
	case IMG_ERR_MODE:
		strcpy(err, "unsupported color mode, only RGB and RGBA supported");
		break;
	default:
		strcpy(err, "unknown error");
	}

	return err;
}

bool load_tga_file(struct tga_file *img, const char *filename)
{
	FILE *fd;
	long img_size;
	int color_mode, i, j;
	float color_swap;
	unsigned char *raw_data;

	if (!(fd = fopen(filename, "rb"))) {
		img_err = IMG_ERR_OPEN;
		return false;
	}

	fseek(fd, 2, SEEK_CUR);

	fread(&img->type, 1, 1, fd);
	if (img->type != 2 && img->type != 3) {
		fclose(fd);
		img_err = IMG_ERR_TYPE;
		return false;
	}

	fseek(fd, 9, SEEK_CUR);

	fread(&img->width, 2, 1, fd);
	fread(&img->height, 2, 1, fd);

	fread(&img->bit_cnt, 1, 1, fd);
	color_mode = img->bit_cnt / 8;
	img->color_mode = color_mode;
	if (color_mode != 3 && color_mode != 4) {
		fclose(fd);
		img_err = IMG_ERR_MODE;
		return false;
	}

	if (color_mode == 3)
		fseek(fd, 1, SEEK_CUR);

	img_size = img->width * img->height * color_mode;
	raw_data = (unsigned char *)malloc(img_size);
	img->data = (float *)malloc(img_size * sizeof(float));
	fread(raw_data, 1, img_size, fd);

	for (i = 0; i < img_size; i += color_mode) {
		for (j = 0; j < color_mode; j++) {
			img->data[i + j] = (float)(raw_data[i + j]) / 255.0f;
		}

		for (j = 0; j < color_mode / 2; j++) {
			color_swap = img->data[i + j];
			img->data[i + j] = img->data[i + color_mode - 1 - j];
			img->data[i + color_mode - 1 - j] = color_swap;
		}
	}

	fclose(fd);

	return true;
}
