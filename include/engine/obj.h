#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>

#include "engine.h"

struct OBJ_FILE {
	size_t v_size;
	size_t f_size;
	float *vs;
	uint  *fs;
};

void load_obj_file(struct OBJ_FILE *file, const char *filename);

#endif // OBJ_H
