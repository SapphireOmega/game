#ifndef MESH_H
#define MESH_H

#include <stdlib.h>

#include "engine.h"
#include "obj.h"

struct simple_vertex {
	float pos[3];
};

struct textured_vertex {
	float pos[3];
	float uv[2];
};

struct face {
	uint indices[3];
	float normal[3];
};

struct simple_mesh {
	uint n_vertices;
	uint n_faces;
	struct simple_vertex *vertices;
	struct face *faces;
};

struct textured_mesh {
	uint n_vertices;
	uint n_faces;
	struct textured_vertex *vertices;
	struct face *faces;
};

struct raw_mesh {
	float *data;
	size_t buf_size;
};

void simple_mesh_from_obj_file(struct simple_mesh *mesh, const struct obj_file *file);

void raw_mesh_from_simple_mesh(struct raw_mesh *res, const struct simple_mesh *mesh);

void destroy_simple_mesh(struct simple_mesh *mesh);
void destroy_raw_mesh(struct raw_mesh *mesh);

#endif // MESH_H
