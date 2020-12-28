#ifndef MESH_H
#define MESH_H

#include <stdlib.h>

#include "engine.h"
#include "obj.h"

struct vertex {
	float pos[3];
};

struct mesh {
	size_t vertices_size;
	size_t faces_size;
	struct vertex *vertices;
	uint *faces;
};

void mesh_from_obj_file(struct mesh *mesh, const struct OBJ_FILE *file);
void destroy_mesh(struct mesh *mesh);

#endif // MESH_H
