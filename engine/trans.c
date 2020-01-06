#include "trans.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

bool
create_vector(vector *vec, unsigned int i)
{
	unsigned int x;

	vec->i = i;
	vec->val = (float *)malloc(sizeof(float) * i);
	if (!vec->val)
		return false;
	for (x = 0; x < i; x++)
		vec->val[x] = 0.0f;

	return true;
}

void
vector_copy_data(vector vec, const float *data)
{
	unsigned int i;

	for (i = 0; i < vec.i; i++)
		vec.val[i] = data[i];
}

bool
vector_scalar_product(vector *res, vector vec, float s)
{
	unsigned int i;

	if (!create_vector(res, vec.i))
		return false;
	for (i = 0; i < res->i; i++)
		res->val[i] = vec.val[i] * s;

	return true;
}

bool
vector_vector_product(float *res, vector vec1, vector vec2)
{
	unsigned int i;

	if (vec1.i != vec2.i)
		return false;
	for (i = 0; i < vec1.i; i++)
		*res += vec1.val[i] * vec2.val[i];

	return true;
}

bool
normalize_vector(vector *res, vector vec)
{
	vector tmp;
	unsigned int i;
	float sum = 0.0f, len; /* sum is the sum of squares */

	if (!create_vector(&tmp, vec.i))
		return false;

	for (i = 0; i < vec.i; i++)
		sum += powf(vec.val[i], 2.0f);
	if (round(sum * 1e7f) == 0.0f) {
		if (!create_vector(res, vec.i))
			return false;
		for (i = 0; i < res->i; i++)
			res->val[i] = 0.0f;
		return true;
	}
	len = sqrtf(sum);
	for (i = 0; i < vec.i; i++)
		tmp.val[i] = vec.val[i] / len;

	res->i = tmp.i;
	res->val = tmp.val;

	return true;
}

bool
create_matrix(matrix *res, unsigned int i, unsigned int j)
{
	unsigned int x;

	res->i = i;
	res->j = j;
	res->val = (float *)malloc(sizeof(float) * i * j);
	if (!res->val)
		return false;
	for (x = 0; x < i * j; x++)
		res->val[x] = 0.0f;

	return true;
}

bool
create_simple_matrix(matrix *res, unsigned int i, unsigned int j, float s)
{
	unsigned int a, x;

	if (!create_matrix(res, i, j))
		return false;

	if (i < j)
		a = i;
	else
		a = j;

	for (x = 0; x < i * j; x++)
		res->val[x] = 0.0f;

	for (x = 0; x < a; x++)
		res->val[x * j + x] = s;

	return true;
}

void
matrix_copy_data(matrix mat, const float *data)
{
	unsigned int i;

	for (i = 0; i < mat.i * mat.j; i++)
		mat.val[i] = data[i];
}

bool
copy_matrix(matrix *res, matrix mat)
{
	if (!create_matrix(res, mat.i, mat.j))
		return false;
	matrix_copy_data(*res, mat.val);

	return true;
}

bool
vector_from_matrix(vector *res, matrix mat, unsigned int i)
{
	unsigned int j;

	if (i < 0 || i >= mat.i)
		return false;
	if (!create_vector(res, mat.j))
		return false;
	for (j = 0; j < mat.j; j++)
		res->val[j] = mat.val[i * mat.j + j];

	return true;
}

bool
matrix_remove_row(matrix *res, matrix mat, unsigned int i)
{
	unsigned int x, y = 0;

	if (i >= mat.i || !create_matrix(res, mat.i - 1, mat.j))
		return false;
	for (x = 0; x < mat.i * mat.j; x++) {
		if (x / mat.j == i)
			continue;
		res->val[y] = mat.val[x];
		y++;
	}

	return true;
}

bool
matrix_remove_column(matrix *res, matrix mat, unsigned int j)
{
	unsigned int x, y = 0;

	if (j >= mat.j || !create_matrix(res, mat.i, mat.j -1))
		return false;
	for (x = 0; x < mat.i * mat.j; x++) {
		if (x % mat.j == j)
			continue;
		res->val[y] = mat.val[x];
		y++;
	}

	return true;
}

bool
matrix_scalar_product(matrix *res, matrix mat, float s)
{
	unsigned int i;

	if (!create_matrix(res, mat.i, mat.j))
		return false;
	for (i = 0; i < mat.i * mat.j; i++)
		res->val[i] = s * mat.val[i];

	return true;
}

bool
matrix_vector_product(vector *res, matrix mat, vector vec)
{
	vector tmp;
	unsigned int i;

	if (vec.i != mat.j)
		return false;
	if (!create_vector(res, vec.i))
		return false;
	for (i = 0; i < vec.i; i++) {
		if (!vector_from_matrix(&tmp, mat, i))
			return false;
		if (!vector_vector_product(&res->val[i], tmp, vec))
			return false;
	}

	return true;
}

bool
matrix_matrix_product(matrix *res, matrix mat1, matrix mat2)
{
	vector tmp1, tmp2;
	matrix test;
	unsigned int i, j;

	if (mat1.j != mat2.i)
		return false;
	if (!create_matrix(res, mat1.i, mat2.j))
		return false;
	for (i = 0; i < res->i; i++) {
		if (!vector_from_matrix(&tmp1, mat1, i))
			return false;
		if (!transpose(&test, mat2)) /* TODO: vector_matrix_product */
			return false;
		if (!matrix_vector_product(&tmp2, test, tmp1))
			return false;
		for (j = 0; j < tmp2.i; j++)
			res->val[i * res->j + j] = tmp2.val[j];
	}

	return true;
}

bool
transpose(matrix *res, matrix mat)
{
	unsigned int i, j;

	if (!create_matrix(res, mat.j, mat.i))
		return false;
	for (i = 0; i < res->i; i++)
		for (j = 0; j < res->j; j++)
			res->val[i * res->j + j] = mat.val[j * mat.j + i];

	return true;
}

static float
det2(matrix mat)
{
	return mat.val[0] * mat.val[3] - mat.val[1] * mat.val[2];
}

static bool
detn(float *res, matrix mat)
{
	unsigned int j;
	matrix tmp1, tmp2;
	float tmp_det;

	if (!matrix_remove_row(&tmp1, mat, 0))
		return false;
	*res = 0.0f;
	for (j = 0; j < mat.j; j++) {
		if (!matrix_remove_column(&tmp2, tmp1, j))
			return false;
		if (!determinant(&tmp_det, tmp2))
			return false;
		if (j % 2 == 0)
			*res += mat.val[j] * tmp_det;
		else
			*res -= mat.val[j] * tmp_det;
	}

	return true;
}

bool
determinant(float *res, matrix mat)
{
	if (mat.i != mat.j || mat.i < 2)
		return false;
	else if (mat.i == 2 && mat.j == 2)
		*res = det2(mat);
	else
		return detn(res, mat);

	return true;
}

bool
matrix_of_minors(matrix *res, matrix mat)
{
	int i;
	matrix tmp;

	if (!create_matrix(res, mat.i, mat.j))
		return false;

	for (i = 0; i < mat.i * mat.j; i++) {
		if (!matrix_remove_row(&tmp, mat, i / mat.j))
			return false;
		if (!matrix_remove_column(&tmp, tmp, i % mat.j))
			return false;
		if (!determinant(&res->val[i], tmp))
			return false;
	}

	return true;
}

bool
inverse_matrix(matrix *res, matrix mat)
{
	matrix minors, cofactors, adjugate;
	float det = 0.0f;
	int i;

	if (!matrix_of_minors(&minors, mat))
		return false;
	if (!copy_matrix(&cofactors, minors))
		return false;
	for (i = 0; i < cofactors.i * cofactors.j; i++) {
		if (i % 2 == (i / cofactors.j + 1) % 2)
			cofactors.val[i] *= -1;
	}
	if (!transpose(&adjugate, cofactors))
		return false;
	for (i = 0; i < cofactors.j; i++)
		det += cofactors.val[i] * mat.val[i];

	return matrix_scalar_product(res, adjugate, 1.0f / det);
}

bool
rotate(matrix *res, matrix mat, float a, vector vec)
{
	matrix rot;

	if (!matrix_from_axis_angle(&rot, a, vec))
		return false;
	if (!matrix_matrix_product(res, mat, rot))
		return false;

	return true;
}

bool
matrix_from_axis_angle(matrix *res, float a, vector vec)
{
	if (vec.i != 3)
		return false;

	const float rres[] = {
		/* row 1 */
		cosf(a) + powf(vec.val[0], 2.0f) * (1.0f - cosf(a)),
		vec.val[0] * vec.val[1] * (1.0f - cosf(a)) - vec.val[2] * sinf(a),
		vec.val[0] * vec.val[2] * (1.0f - cosf(a)) + vec.val[1] * sinf(a),
		0.0f,
		/* row 2 */
		vec.val[1] * vec.val[0] * (1.0f - cosf(a)) + vec.val[2] * sinf(a),
		cosf(a) + powf(vec.val[1], 2.0f) * (1.0f - cosf(a)),
		vec.val[1] * vec.val[2] * (1.0f - cosf(a)) - vec.val[0] * sinf(a),
		0.0f,
		/* row 3 */
		vec.val[2] * vec.val[0] * (1.0f - cosf(a)) - vec.val[1] * sinf(a),
		vec.val[2] * vec.val[1] * (1.0f - cosf(a)) + vec.val[0] * sinf(a),
		cosf(a) + powf(vec.val[2], 2.0f) * (1.0f - cosf(a)),
		0.0f,
		/* row 4 */
		0.0f, 0.0f, 0.0f, 1.0f
	};

	if (!create_matrix(res, 4, 4))
		return false;
	matrix_copy_data(*res, rres);

	return true;
}

void
print_vector(vector vec)
{
	unsigned int i;

	for (i = 0; i < vec.i; i++)
		printf("%f\n", vec.val[i]);
}

void
print_matrix(matrix mat)
{
	unsigned int i;
	unsigned int j;

	for (i = 0; i < mat.i; i++) {
		for (j = 0; j < mat.j; j++)
			printf("%f ", mat.val[i * mat.j + j]);
		printf("\n");
	}
}
