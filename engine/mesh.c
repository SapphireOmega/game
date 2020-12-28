#include "mesh.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

void simple_mesh_from_obj_file(struct simple_mesh *mesh, const struct obj_file *file)
{
	mesh->n_vertices = file->vs_size / 3;
	mesh->vertices = (struct simple_vertex *)malloc(mesh->n_vertices * sizeof(struct simple_vertex));
	if (!mesh->vertices) {
		fprintf(stderr, "Error allocating vertices while creating mesh: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	mesh->n_faces = file->fs_size / 3;
	mesh->faces = (struct face *)malloc(mesh->n_faces * sizeof(struct face));
	if (!mesh->faces) {
		fprintf(stderr, "Error allocating faces while creating mesh: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	for (uint i = 0; i < mesh->n_vertices; i++) {
		mesh->vertices[i].pos[0] = file->vs[i * 3];
		mesh->vertices[i].pos[1] = file->vs[i * 3 + 1];
		mesh->vertices[i].pos[2] = file->vs[i * 3 + 2];
	}

	for (uint i = 0; i < mesh->n_faces; i++) {
		mesh->faces[i].indices[0] = file->fs[i * 3] - 1;
		mesh->faces[i].indices[1] = file->fs[i * 3 + 1] - 1;
		mesh->faces[i].indices[2] = file->fs[i * 3 + 2] - 1;
	}

	if (file->vns_size != 0) {
		for (uint i = 0; i < mesh->n_faces; i++) {
			mesh->faces[i].normal[0] = file->vns[(file->vns_indices[i] - 1) * 3];
			mesh->faces[i].normal[1] = file->vns[(file->vns_indices[i] - 1) * 3 + 1];
			mesh->faces[i].normal[2] = file->vns[(file->vns_indices[i] - 1) * 3 + 2];
		}

		printf("vertices:\n");
		for (uint i = 0; i < mesh->n_vertices; i++)
			printf("%f, %f, %f\n", mesh->vertices[i].pos[0], mesh->vertices[i].pos[1], mesh->vertices[i].pos[2]);
		printf("faces:\n");
		for (uint i = 0; i < mesh->n_faces; i++)
			printf("indices: %d, %d, %d, normal: %f, %f, %f\n",
			       mesh->faces[i].indices[0], mesh->faces[i].indices[1], mesh->faces[i].indices[2],
			       mesh->faces[i].normal[0], mesh->faces[i].normal[1], mesh->faces[i].normal[2]);
	} else {
		for (uint i = 0; i < mesh->n_faces; i++) {
			mesh->faces[i].normal[0] = 1.0f;
			mesh->faces[i].normal[1] = 0.0f;
			mesh->faces[i].normal[2] = 0.0f;
		}
	}
}

void raw_mesh_from_simple_mesh(struct raw_mesh *res, const struct simple_mesh *mesh)
{
	res->buf_size = 18 * mesh->n_faces * sizeof(float); // 18 = (3 + 3) * 3
	res->data = (float *)malloc(res->buf_size);
	if (!res) {
		fprintf(stderr, "Error allocating memory for raw mesh: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	for (uint fi = 0; fi < mesh->n_faces; fi++) { // fi = face index
		for (uint vi = 0; vi < 3; vi++) { // vi = vertex index
			for (uint pi = 0; pi < 3; pi++) // pi = position index
				res->data[fi * 18 + vi * 6 + pi] = mesh->vertices[mesh->faces[fi].indices[vi]].pos[pi];
			for (uint ni = 0; ni < 3; ni++) // ni = normal index
				res->data[fi * 18 + vi * 6 + 3 + ni] = mesh->faces[fi].normal[ni];
		}
	}
}

void destroy_simple_mesh(struct simple_mesh *mesh)
{
	free(mesh->vertices);
	free(mesh->faces);
}

void destroy_raw_mesh(struct raw_mesh *mesh)
{
	free(mesh->data);
}
