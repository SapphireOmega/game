#ifndef IMGLOAD_H
#define IMGLOAD_H

#include <stdbool.h>

enum img_err { IMG_ERR_NONE, IMG_ERR_OPEN, IMG_ERR_TYPE, IMG_ERR_MODE };
enum color_mode { IMG_G = 1, IMG_GA = 2, IMG_RGB = 3, IMG_RGBA = 4 };

struct tga_file {
	unsigned char type;
	short int width;
	short int height;
	unsigned char bit_cnt;
	float *data;
	enum color_mode color_mode;
};

enum img_err img_err = IMG_ERR_NONE;

char *img_strerror(enum img_err img_err);
bool load_tga_file(struct tga_file *img, const char *filename);

#endif
