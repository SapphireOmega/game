#ifndef OBJ_H
#define OBJ_H

#include <stdlib.h>
#include <stdbool.h>

#include "engine.h"

struct obj_file {
	size_t vs_size;
	size_t vns_size;
	size_t fs_size;
	float  *vs;
	float  *vns;
	uint   *fs;
	uint   *vns_indices;
};

void load_obj_file(struct obj_file *file, const char *filename);

void destroy_obj_file(struct obj_file *file);

#endif // OBJ_H
