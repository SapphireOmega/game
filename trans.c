#include "trans.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

vector *
create_vector(int i)
{
	vector *vec;
	int x;

	vec = (vector *)malloc(sizeof(vec));
	if (!vec)
		return NULL;
	vec->i = i;
	vec->val = (float *)malloc(sizeof(float) * i);
	if (!vec->val)
		return NULL;
	for (x = 0; x < i; x++)
		vec->val[x] = 0.0f;

	return vec;
}

void
copy_vector_data(vector *vec, const float *data)
{
	int i;

	for (i = 0; i < vec->i; i++)
		vec->val[i] = data[i];
}

vector *
vector_scalar_product(const vector *vec, float s)
{
	int i;
	vector *new_vec;

	new_vec = create_vector(vec->i);
	if (!new_vec)
		return NULL;
	for (i = 0; i < new_vec->i; i++)
		new_vec->val[i] = vec->val[i] * s;

	return new_vec;
}

bool
vector_vector_product(float *res, const vector *vec1, const vector *vec2)
{
	int i;

	if (vec1->i != vec2->i)
		return false;
	for (i = 0; i < vec1->i; i++)
		*res += vec1->val[i] * vec2->val[i];

	return true;
}

vector *
normalize_vector(const vector *vec)
{
	vector *res;
	int i;
	float sum, len; /* sum is the sum of squares */

	res = create_vector(vec->i);
	sum = 0.0f;

	for (i = 0; i < vec->i; i++)
		sum += powf(vec->val[i], 2.0f);
	len = sqrtf(sum);
	for (i = 0; i < vec->i; i++)
		res->val[i] = vec->val[i] / len;

	return res;
}

matrix *
create_matrix(int i, int j)
{
	int x;
	matrix *mat;

	mat = (matrix *)malloc(sizeof(mat));
	if (!mat)
		return NULL;
	mat->i = i;
	mat->j = j;
	mat->val = (float *)malloc(sizeof(float) * i * j);
	if (!mat->val)
		return NULL;
	for (x = 0; x < i * j; x++)
		mat->val[x] = 0.0f;

	return mat;
}

matrix *
create_simple_matrix(int i, int j, float s)
{
	matrix *res;
	int a, x;

	res = create_matrix(i, j);
	if (!res)
		return NULL;

	if (i < j)
		a = i;
	else
		a = j;

	for (x = 0; x < i * j; x++)
		res->val[x] = 0.0f;

	for (x = 0; x < a; x++)
		res->val[x * j + x] = s;

	return res;
}

void
copy_matrix_data(matrix *mat, const float *data)
{
	int i;

	for (i = 0; i < mat->i * mat->j; i++)
		mat->val[i] = data[i];
}

vector *
vector_from_matrix(const matrix *mat, int i)
{
	vector *res;
	int j;

	if (i < 0 || i >= mat->i)
		return NULL;
	res = create_vector(mat->j);
	if (!res)
		return NULL;
	for (j = 0; j < mat->j; j++)
		res->val[j] = mat->val[i * mat->j + j];

	return res;
}

matrix *
matrix_scalar_product(const matrix *mat, float s)
{
	matrix *res;
	int i;

	res = create_matrix(mat->i, mat->j);
	if (!res)
		return NULL;
	for (i = 0; i < mat->i * mat->j; i++)
		res->val[i] = s * mat->val[i];

	return res;
}

vector *
matrix_vector_product(const matrix *mat, const vector *vec)
{
	vector *res, *tmp;
	int i;

	if (vec->i != mat->j)
		return NULL;
	res = create_vector(vec->i);
	if (!res)
		return NULL;
	for (i = 0; i < vec->i; i++) {
		tmp = vector_from_matrix(mat, i);
		if (!vector_vector_product(&res->val[i], tmp, vec))
			return NULL;
	}

	return res;
}

matrix *
matrix_matrix_product(const matrix *mat1, const matrix *mat2)
{
	matrix *res;
	vector *tmp;
	int i, j;

	if (mat1->j != mat2->i)
		return NULL;
	res = create_matrix(mat1->i, mat2->j);
	if (!res)
		return NULL;
	for (i = 0; i < res->i; i++) {
		tmp = matrix_vector_product(mat2, vector_from_matrix(mat1, i));
		for (j = 0; j < tmp->i; j++)
			res->val[i * res->j + j] = tmp->val[j];
	}

	return res;
}

matrix *
transpose(const matrix *mat)
{
	matrix *res;
	int i, j;

	res = create_matrix(mat->j, mat->i);
	if (!res)
		return NULL;
	for (i = 0; i < res->i; i++)
		for (j = 0; j < res->j; j++)
			res->val[i * res->j + j] = mat->val[j * mat->j + i];

	return res;
}

matrix *
rotate(const matrix *mat, float a, const vector *vec)
{
	// matrix *res;
	// matrix *rotx;
	// matrix *roty;
	// matrix *rotz;

	// rotx = create_matrix(4, 4);
	// roty = create_matrix(4, 4);
	// rotz = create_matrix(4, 4);
	// if (!vec || vec->i != 3 || !rotx || !roty || !rotz)
	// 	return NULL;

	// const float rrotx[] = {
	// 	1.0f, 0.0f, 0.0f, 0.0f,
	// 	0.0f, cosf(angle * vec->val[0]), -sinf(angle * vec->val[0]), 0.0f,
	// 	0.0f, sinf(angle * vec->val[0]), cosf(angle * vec->val[0]), 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f,
	// };

	// const float rroty[] = {
	// 	cosf(angle * vec->val[1]), 0.0f, sinf(angle * vec->val[1]), 0.0f,
	// 	0.0f, 1.0f, 0.0f, 0.0f,
	// 	-sinf(angle * vec->val[1]), 0.0f, cosf(angle * vec->val[1]), 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f,
	// };

	// const float rrotz[] = {
	// 	cosf(angle * vec->val[2]), -sinf(angle * vec->val[2]), 0.0f, 0.0f,
	// 	sinf(angle * vec->val[2]), cosf(angle * vec->val[2]), 0.0f, 0.0f,
	// 	0.0f, 0.0f, 1.0f, 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f,
	// };

	// copy_matrix_data(rotx, rrotx);
	// copy_matrix_data(roty, rroty);
	// copy_matrix_data(rotz, rrotz);

	// res = matrix_matrix_product(roty, rotz);
	// res = matrix_matrix_product(res, rotx);
	// res = matrix_matrix_product(res, mat);
	
	matrix *res;
	matrix *rot;

	rot = matrix_from_axis_angle(a, vec);
	res = matrix_matrix_product(mat, rot);

	return res;
}

matrix *
matrix_from_axis_angle(float a, const vector *vec)
{
	matrix *res;

	if (vec->i != 3)
		return NULL;

	const float rres[] = {
		/* row 1 */
		cosf(a) + powf(vec->val[0], 2.0f) * (1.0f - cosf(a)),
		vec->val[0] * vec->val[1] * (1.0f - cosf(a)) - vec->val[2] * sinf(a),
		vec->val[0] * vec->val[2] * (1.0f - cosf(a)) + vec->val[1] * sinf(a),
		0.0f,
		/* row 2 */
		vec->val[1] * vec->val[0] * (1.0f - cosf(a)) + vec->val[2] * sinf(a),
		cosf(a) + powf(vec->val[1], 2.0f) * (1.0f - cosf(a)),
		vec->val[1] * vec->val[2] * (1.0f - cosf(a)) - vec->val[0] * sinf(a),
		0.0f,
		/* row 3 */
		vec->val[2] * vec->val[0] * (1.0f - cosf(a)) - vec->val[1] * sinf(a),
		vec->val[2] * vec->val[1] * (1.0f - cosf(a)) + vec->val[0] * sinf(a),
		cosf(a) + powf(vec->val[2], 2.0f) * (1.0f - cosf(a)),
		0.0f,
		/* row 4 */
		0.0f, 0.0f, 0.0f, 1.0f
	};

	res = create_matrix(4, 4);
	if (!res)
		return NULL;
	copy_matrix_data(res, rres);

	return res;
}

void
print_vector(const vector *vec)
{
	int i;

	for (i = 0; i < vec->i; i++)
		printf("%f\n", vec->val[i]);
}

void
print_matrix(const matrix *mat)
{
	int i;
	int j;

	for (i = 0; i < mat->i; i++) {
		for (j = 0; j < mat->j; j++)
			printf("%f ", mat->val[i * mat->j + j]);
		printf("\n");
	}
}
