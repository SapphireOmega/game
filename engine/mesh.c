#include "mesh.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

void mesh_from_obj_file(struct mesh *mesh, const struct OBJ_FILE *file)
{
	mesh->vertices_size = file->v_size / 3;
	mesh->vertices = (struct vertex *)malloc(mesh->vertices_size * sizeof(struct vertex));
	if (!mesh->vertices) {
		fprintf(stderr, "Error allocating vertices while creating mesh: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	mesh->faces_size = file->f_size;
	mesh->faces = (uint *)malloc(mesh->faces_size * sizeof(uint));
	if (!mesh->faces) {
		fprintf(stderr, "Error allocating faces while creating mesh: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	for (uint i = 0; i < mesh->vertices_size; i++) {
		mesh->vertices[i].pos[0] = file->vs[i * 3];
		mesh->vertices[i].pos[1] = file->vs[i * 3 + 1];
		mesh->vertices[i].pos[2] = file->vs[i * 3 + 2];
	}

	for (uint i = 0; i < mesh->faces_size; i++)
		mesh->faces[i] = file->fs[i] - 1;
}

void destroy_mesh(struct mesh *mesh)
{
	free(mesh->vertices);
	free(mesh->faces);
}
