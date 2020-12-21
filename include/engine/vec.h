#ifndef VEC_H
#define VEC_H

#include "engine.h"

void print_vector(uint vec_size, float vec[vec_size]);
void print_matrix(uint rows, uint columns, float matrix[rows][columns]);
void normalize(uint vec_size, float res[vec_size], float vec[vec_size]);
void scalar_vector_product(uint vec_size, float res[vec_size], float scalar, float vec[vec_size]);
float dot(uint vec_size, float vec1[vec_size], float vec2[vec_size]);
void transpose(uint rows, uint columns, float res[columns][rows], float mat[rows][columns]);
void scalar_matrix_product(uint rows, uint columns, float res[rows][columns], float scalar, float mat[rows][columns]);
void matrix_vector_product(uint rows, uint columns, float res[columns], float mat[rows][columns], float vec[rows]);
void vector_matrix_product(uint rows, uint columns, float res[rows], float vec[columns], float mat[rows][columns]);
void matrix_matrix_product(
	uint res_rows, uint res_columns, uint common_size,
	float res[res_rows][res_columns],
	float mat1[res_rows][common_size],
	float mat2[common_size][res_columns]);
void remove_row(
	uint mat_rows, uint mat_columns,
	float res[mat_rows - 1][mat_columns],
	float mat[mat_rows][mat_columns],
	uint row);
void remove_column(
	uint mat_rows, uint mat_columns,
	float res[mat_rows][mat_columns - 1],
	float mat[mat_rows][mat_columns],
	uint column);
void remove_row_column(
	uint mat_rows, uint mat_columns,
	float res[mat_rows - 1][mat_columns - 1],
	float mat[mat_rows][mat_columns],
	uint row, uint column);
float det(uint mat_size, float mat[mat_size][mat_size]);
void matrix_of_minors(uint mat_size, float res[mat_size][mat_size], float mat[mat_size][mat_size]);
void apply_checker_pattern(uint rows, uint columns, float res[rows][columns], float mat[rows][columns]);
float det_from_gofactors(uint mat_size, float mat[mat_size][mat_size], float cofactors[mat_size][mat_size]);
void inverse(uint mat_size, float res[mat_size][mat_size], float mat[mat_size][mat_size]);
void add_row(
	uint mat_rows, uint mat_columns,
	float res[mat_rows + 1][mat_columns],
	float mat[mat_rows][mat_columns],
	float row[mat_columns]);
void add_column(
	uint mat_rows, uint mat_columns,
	float res[mat_rows][mat_columns + 1],
	float mat[mat_rows][mat_columns],
	float column[mat_rows]);
void homogeneous(uint mat_size, float res[mat_size + 1][mat_size + 1], float mat[mat_size][mat_size]);
void rotation_3d(float res[3][3], float axis[3], float angle);
void rotation_3d_homogeneous(float res[4][4], float axis[3], float angle);
void identity(uint rows, uint columns, float res[rows][columns]);
void add_translation(
	uint dimension,
	float res[dimension + 1][dimension + 1],
	float mat[dimension + 1][dimension + 1],
	float vec[dimension]);
void translation(uint dimension, float res[dimension + 1][dimension + 1], float vec[dimension]);
void scale(uint dimensions, float res[dimensions][dimensions], float vec[dimensions]);
void scale_homogeneous(uint dimensions, float res[dimensions + 1][dimensions + 1], float vec[dimensions]);

#endif /* VEC_H */
