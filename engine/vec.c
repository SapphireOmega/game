#include "vec.h"

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>

void print_vector(uint vec_size, float vec[vec_size])
{
	printf("( ");
	for (uint i = 0; i < vec_size; i++)
		printf("%f ", vec[i]);
	printf(")\n");
}

void print_matrix(uint rows, uint columns, float matrix[rows][columns])
{
	printf("(\n");
	for (uint i = 0; i < rows; i++) {
		printf("\t");
		print_vector(columns, matrix[i]);
	}
	printf(")\n");
}

void normalize(uint vec_size, float res[vec_size], float vec[vec_size])
{
	float sum = 0.0f;
	for (uint i = 0; i < vec_size; i++)
		sum += fabsf(vec[i]) * fabsf(vec[i]);
	float len = sqrtf(sum);
	if (sum == 0.0f)
		for (uint i = 0; i < vec_size; i++)
			res[i] = vec[i];
	else
		for (uint i = 0; i < vec_size; i++)
			res[i] = vec[i] / len;
}

void scalar_vector_product(uint vec_size, float res[vec_size], float scalar, float vec[vec_size])
{
	for (uint i = 0; i < vec_size; i++)
		res[i] = vec[i] * scalar;
}

float dot(uint vec_size, float vec1[vec_size], float vec2[vec_size])
{
	float res = 0.0f;
	for (uint i = 0; i < vec_size; i++)
		res += vec1[i] * vec2[i];
	return res;
}

void transpose(uint rows, uint columns, float res[columns][rows], float mat[rows][columns])
{
#ifdef DEBUG
	assert(res != mat);
#endif
	for (uint j = 0; j < rows; j++)
		for (uint i = 0; i < columns; i++)
			res[i][j] = mat[j][i];
}

void scalar_matrix_product(uint rows, uint columns, float res[rows][columns], float scalar, float mat[rows][columns])
{
	for (uint i = 0; i < rows; i++)
		scalar_vector_product(columns, res[i], scalar, mat[i]);
}

void matrix_vector_product(uint rows, uint columns, float res[columns], float mat[rows][columns], float vec[rows])
{
#ifdef DEBUG
	assert(res != vec);
#endif
	for (uint i = 0; i < rows; i++)
		res[i] = dot(columns, mat[i], vec);
}

void vector_matrix_product(uint rows, uint columns, float res[rows], float vec[columns], float mat[rows][columns])
{
#ifdef DEBUG
	assert(res != vec);
#endif
	float tmp[columns][rows];
	transpose(rows, columns, tmp, mat);
	matrix_vector_product(columns, rows, res, tmp, vec);
}

void matrix_matrix_product(
	uint res_rows, uint res_columns, uint common_size,
	float res[res_rows][res_columns],
	float mat1[res_rows][common_size],
	float mat2[common_size][res_columns])
{
#ifdef DEBUG
	assert(res != mat1);
	assert(res != mat2);
#endif
	for (uint j = 0; j < res_rows; j++)
		for (uint i = 0; i < common_size; i++)
			vector_matrix_product(common_size, res_columns, res[j], mat1[j], mat2);
}

void remove_row(
	uint mat_rows, uint mat_columns,
	float res[mat_rows - 1][mat_columns],
	float mat[mat_rows][mat_columns],
	uint row)
{
#ifdef DEBUG
	assert(res != mat);
	assert(row < mat_rows);
#endif
	uint res_index = 0;
	for (uint j = 0; j < mat_rows; j++) {
		if (j == row)
			continue;
		for (uint i = 0; i < mat_columns; i++)
			res[res_index][i] = mat[j][i];
		res_index++;
	}
}

void remove_column(
	uint mat_rows, uint mat_columns,
	float res[mat_rows][mat_columns - 1],
	float mat[mat_rows][mat_columns],
	uint column)
{
#ifdef DEBUG
	assert(res != mat);
	assert(column < mat_columns);
#endif
	for (uint j = 0; j < mat_rows; j++) {
		uint res_index = 0;
		for (uint i = 0; i < mat_columns; i++) {
			if (i == column)
				continue;
			res[j][res_index] = mat[j][i];
			res_index++;
		}
	}
}

void remove_row_column(
	uint mat_rows, uint mat_columns,
	float res[mat_rows - 1][mat_columns - 1],
	float mat[mat_rows][mat_columns],
	uint row, uint column)
{
#ifdef DEBUG
	assert(res != mat);
	assert(row < mat_rows);
	assert(column < mat_columns);
#endif
	uint res_j = 0;
	for (uint j = 0; j < mat_rows; j++) {
		uint res_i = 0;
		if (j == row)
			continue;
		for (uint i = 0; i < mat_columns; i++) {
			if (i == column)
				continue;
			res[res_j][res_i] = mat[j][i];
			res_i++;
		}
		res_j++;
	}
}

float det(uint mat_size, float mat[mat_size][mat_size])
{
#ifdef debug
	assert(mat_size > 1);
#endif
	float res = 0.0f;
	if (mat_size == 2) {
		res = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
	} else {
		for (uint i = 0; i < mat_size; i++) {
			int sign = i % 2 * -2 + 1;
			float sub[mat_size - 1][mat_size - 1];
			remove_row_column(mat_size, mat_size, sub, mat, 0, i);
			res += sign * mat[0][i] * det(mat_size - 1, sub);
		}
	}
	return res;
}

void matrix_of_minors(uint mat_size, float res[mat_size][mat_size], float mat[mat_size][mat_size])
{
#ifdef DEBUG
	assert(res != mat);
#endif
	for (uint j = 0; j < mat_size; j++) {
		for (uint i = 0; i < mat_size; i++) {
			float sub[mat_size - 1][mat_size - 1];
			remove_row_column(mat_size, mat_size, sub, mat, j, i);
			res[j][i] = det(mat_size - 1, sub);
		}
	}
}

void apply_checker_pattern(uint rows, uint columns, float res[rows][columns], float mat[rows][columns])
{
	for (uint j = 0; j < rows; j++) {
		uint row_multiplier = j % 2 * -2 + 1;
		for (uint i = 0; i < columns; i++) {
			int sign = (i % 2 * -2 + 1) * row_multiplier;
			res[j][i] = (float)sign * mat[j][i];
		}
	}
}

float det_from_cofactors(uint mat_size, float mat[mat_size][mat_size], float cofactors[mat_size][mat_size])
{
	float res = 0.0f;
	for (uint i = 0; i < mat_size; i++)
		res += mat[0][i] * cofactors[0][i];
	return res;
}

void inverse(uint mat_size, float res[mat_size][mat_size], float mat[mat_size][mat_size])
{
	float cofactors[mat_size][mat_size];
	float adjugate[mat_size][mat_size];

	matrix_of_minors(mat_size, cofactors, mat);
	apply_checker_pattern(mat_size, mat_size, cofactors, cofactors);
	transpose(mat_size, mat_size, adjugate, cofactors);
	scalar_matrix_product(mat_size, mat_size, res, 1.0f / det_from_cofactors(mat_size, mat, cofactors), adjugate);
}

void add_row(
	uint mat_rows, uint mat_columns,
	float res[mat_rows + 1][mat_columns],
	float mat[mat_rows][mat_columns],
	float row[mat_columns])
{
#ifdef DEBUG
	assert(res != mat);
#endif
	for (uint j = 0; j < mat_rows; j++)
		for (uint i = 0; i < mat_columns; i++)
			res[j][i] = mat[j][i];
	for (uint i = 0; i < mat_columns; i++)
		res[mat_rows][i] = row[i];
}

void add_column(
	uint mat_rows, uint mat_columns,
	float res[mat_rows][mat_columns + 1],
	float mat[mat_rows][mat_columns],
	float column[mat_rows])
{
#ifdef DEBUG
	assert(res != mat);
#endif
	for (uint j = 0; j < mat_rows; j++) {
		for (uint i = 0; i < mat_columns; i++)
			res[j][i] = mat[j][i];
		res[j][mat_columns] = column[j];
	}
}

void homogeneous(uint mat_size, float res[mat_size + 1][mat_size + 1], float mat[mat_size][mat_size])
{
#ifdef DEBUG
	assert(res != mat);
#endif
	for (uint j = 0; j < mat_size; j++) {
		for (uint i = 0; i < mat_size; i++)
			res[j][i] = mat[j][i];
		res[j][mat_size] = 0.0f;
	}
	for (uint i = 0; i < mat_size; i++)
		res[mat_size][i] = 0.0f;
	res[mat_size][mat_size] = 1.0f;
}

void rotation_3d(float res[3][3], float axis[3], float angle)
{
	float sin_a = sinf(angle);
	float cos_a = cosf(angle);

	res[0][0] = cos_a + axis[0] * axis[0] * (1.0f - cos_a);
	res[0][1] = axis[0] * axis[1] * (1.0f - cos_a) - axis[2] * sin_a;
	res[0][2] = axis[0] * axis[2] * (1.0f - cos_a) + axis[1] * sin_a;

	res[1][0] = axis[1] * axis[0] * (1.0f - cos_a) + axis[2] * sin_a;
	res[1][1] = cos_a + axis[1] * axis[1] * (1.0f - cos_a);
	res[1][2] = axis[1] * axis[2] * (1.0f - cos_a) - axis[0] * sin_a;

	res[2][0] = axis[2] * axis[0] * (1.0f - cos_a) - axis[1] * sin_a;
	res[2][1] = axis[2] * axis[1] * (1.0f - cos_a) + axis[0] * sin_a;
	res[2][2] = cos_a + axis[2] * axis[2] * (1.0f - cos_a);
}

void rotation_3d_homogeneous(float res[4][4], float axis[3], float angle)
{
	float sin_a = sinf(angle);
	float cos_a = cosf(angle);

	res[0][0] = cos_a + axis[0] * axis[0] * (1.0f - cos_a);
	res[0][1] = axis[0] * axis[1] * (1.0f - cos_a) - axis[2] * sin_a;
	res[0][2] = axis[0] * axis[2] * (1.0f - cos_a) + axis[1] * sin_a;
	res[0][3] = 0.0f;

	res[1][0] = axis[1] * axis[0] * (1.0f - cos_a) + axis[2] * sin_a;
	res[1][1] = cos_a + axis[1] * axis[1] * (1.0f - cos_a);
	res[1][2] = axis[1] * axis[2] * (1.0f - cos_a) - axis[0] * sin_a;
	res[1][3] = 0.0f;

	res[2][0] = axis[2] * axis[0] * (1.0f - cos_a) - axis[1] * sin_a;
	res[2][1] = axis[2] * axis[1] * (1.0f - cos_a) + axis[0] * sin_a;
	res[2][2] = cos_a + axis[2] * axis[2] * (1.0f - cos_a);
	res[2][3] = 0.0f;

	res[3][0] = 0.0f;
	res[3][1] = 0.0f;
	res[3][2] = 0.0f;
	res[3][3] = 1.0f;
}

void identity(uint rows, uint columns, float res[rows][columns])
{
	for (uint j = 0; j < rows; j++) {
		for (uint i = 0; i < rows; i++) {
			if (j == i)
				res[j][i] = 1.0f;
			else
				res[j][i] = 0.0f;
		}
	}
}

void add_translation(
	uint dimension,
	float res[dimension + 1][dimension + 1],
	float mat[dimension + 1][dimension + 1],
	float vec[dimension])
{
	if (res != mat)
		for (uint j = 0; j < dimension + 1; j++)
			for (uint i = 0; i < dimension + 1; i++)
				res[j][i] = mat[j][i];
	for (uint j = 0; j < dimension; j++)
		res[j][dimension] = vec[j];
}

void translation(uint dimension, float res[dimension + 1][dimension + 1], float vec[dimension])
{
	identity(dimension + 1, dimension + 1, res);
	add_translation(dimension, res, res, vec);
}

void scale(uint dimensions, float res[dimensions][dimensions], float vec[dimensions])
{
	for (uint j = 0; j < dimensions; j++) {
		for (uint i = 0; i < dimensions; i++) {
			if (j == i)
				res[j][i] = vec[i];
			else
				res[j][i] = 0.0f;
		}
	}
}

void scale_homogeneous(uint dimensions, float res[dimensions + 1][dimensions + 1], float vec[dimensions])
{
	for (uint j = 0; j < dimensions; j++) {
		for (uint i = 0; i < dimensions; i++) {
			if (j == i)
				res[j][i] = vec[i];
			else
				res[j][i] = 0.0f;
		}
	}
	for (uint i = 0; i < dimensions; i++)
		res[dimensions][i] = 0.0f;
	res[dimensions][dimensions] = 1.0f;
}
