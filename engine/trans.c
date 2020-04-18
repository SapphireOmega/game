#include "util.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#include "trans.h"

static float *
create_dynamic_unsafe(uint count, va_list va)
{
	float *val;
	uint x;

	val = (float *)malloc(sizeof(float) * count);
	assert(val != NULL);
	
	for (x = 0; x < count; x++)
		val[x] = (float)va_arg(va, double);
	
	return val;
}

Vector
create_dynamic_vector_unsafe(uint i, ...)
{
	Vector res;
	va_list va;

	va_start(va, i);
	res = (Vector){ i, create_dynamic_unsafe(i, va) };
	va_end(va);

	return res;
}

Matrix
create_dynamic_matrix_unsafe(uint i, uint j, ...)
{
	Matrix res;
	va_list va;

	va_start(va, j);
	res = (Matrix){ i, j, create_dynamic_unsafe(i * j, va) };
	va_end(va);

	return res;
}

static float *
create_dynamic_empty(uint count)
{
	float *val;
	uint i;

	val = (float *)malloc(sizeof(float) * count);
	assert(val != NULL);
	for (i = 0; i < count; i++)
		val[i] = 0.0f;

	return val;
}

Vector
create_dynamic_vector_empty(uint i)
{
	return (Vector){ i, create_dynamic_empty(i) };
}

Matrix
create_dynamic_matrix_empty(uint i, uint j)
{
	return (Matrix){ i, j, create_dynamic_empty(i * j) };
}

void
vector_copy(Vector res, Vector vec)
{
	uint i;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == vec.i);
#endif

	for (i = 0; i < vec.i; i++)
		res.val[i] = vec.val[i];
}

void
matrix_copy(Matrix res, Matrix mat)
{
	uint i;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
#endif

	for (i = 0; i < mat.i * mat.j; i++)
		res.val[i] = mat.val[i];
}

void
normalized_vector(Vector res, Vector vec)
{
	uint i;
	float total = 0.0f, len;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == vec.i);
#endif

	for (i = 0; i < vec.i; i++)
		total += powf(vec.val[i], 2.0f);
	if ((len = sqrtf(total)) == 0.0f)
		for (i = 0; i < res.i; i++)
			res.val[i] = 0.0f;
	else
		for (i = 0; i < vec.i; i++)
			res.val[i] = vec.val[i] / len;
}

void
vector_scalar_product(Vector res, Vector vec, float s)
{
	uint i;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == vec.i);
#endif

	for (i = 0; i < res.i; i++)
		res.val[i] = vec.val[i] * s;
}

float
vector_vector_product(Vector vec1, Vector vec2)
{
	uint i;
	float total = 0.0f;

#ifdef DEBUG
	assert(vec1.val != NULL);
	assert(vec2.val != NULL);
	assert(vec1.i == vec2.i);
#endif

	for (i = 0; i < vec1.i; i++)
		total += vec1.val[i] * vec2.val[i];

	return total;
}

void
matrix_scalar_product(Matrix res, Matrix mat, float s)
{
	uint i;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
#endif

	for (i = 0; i < mat.i * mat.j; i++)
		res.val[i] = s * mat.val[i];
}

void
matrix_vector_product(Vector res, Matrix mat, Vector vec)
{
	Matrix mvec, mres;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(vec.val != NULL);
	assert(vec.i == mat.j);
	assert(res.i == mat.i);
	assert(res.val != vec.val);
#endif

	mvec = (Matrix){ .i = vec.i, .j = 1, .val = vec.val };
	mres = (Matrix){ .i = res.i, .j = 1, .val = res.val };

	matrix_matrix_product(mres, mat, mvec);
}

void
matrix_matrix_product(Matrix res, Matrix m1, Matrix m2)
{
	uint x, y, z;
	float *val;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(m1.val != NULL);
	assert(m2.val != NULL);
	assert(res.i == m1.i);
	assert(res.j == m2.j);
	assert(m1.j == m2.i);
	assert(res.val != m1.val);
	assert(res.val != m2.val);
#endif

	for (x = 0; x < m1.i; x++) {
		for (y = 0; y < m2.j; y++) {
			val = &res.val[x * res.j + y];
			*val = 0.0f;
			for (z = 0; z < m1.j; z++)
				*val += m1.val[x * m1.j + z] * \
				        m2.val[z * m2.j + y];
		}
	}
}

void
matrix_multiply_vector_dynamic(Matrix mat, Vector res)
{
	Vector tmp;

#ifdef DEBUG
	assert(vec.val != NULL);
#endif

	tmp = create_dynamic_vector_empty(res.i);

	matrix_vector_product(tmp, mat, res);
	vector_copy(res, tmp);

	destroy_dynamic_vector(tmp);
}

void
matrix_multiply_matrix_dynamic(Matrix mat, Matrix res)
{
	Matrix tmp;

#ifdef DEBUG
	assert(mat.val != NULL);
#endif

	tmp = create_dynamic_matrix_empty(mat.i, mat.j);

	matrix_matrix_product(tmp, mat, res);
	matrix_copy(res, tmp);

	destroy_dynamic_matrix(tmp);
}

void
vector_from_matrix(Vector res, Matrix mat, uint i)
{
	uint j;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(i < mat.i);
	assert(res.i == mat.j);
#endif

	for (j = 0; j < mat.j; j++)
		res.val[j] = mat.val[i * mat.j + j];
}

/* Only used by matrix_remove_row() and matrix_remove_column() */
#define MATRIX_REMOVE(X)                                                       \
        {                                                                      \
                uint x, y = 0;                                                 \
                for (x = 0; x < mat.i * mat.j; x++) {                          \
                        if (X)                                                 \
                                continue;                                      \
                        res.val[y] = mat.val[x];                               \
                        y++;                                                   \
                }                                                              \
        }

void
matrix_remove_row(Matrix res, Matrix mat, uint i)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(i < mat.i);
	assert(res.i == mat.i - 1);
	assert(res.j == mat.j);
#endif

	MATRIX_REMOVE(x / mat.j == i);
}

void
matrix_remove_column(Matrix res, Matrix mat, uint j)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(j < mat.j);
	assert(res.i == mat.i);
	assert(res.j == mat.j - 1);
#endif

	MATRIX_REMOVE(x % mat.j == j);
}

/* This function only gets called from determinant() */
static inline float
det(Matrix mat)
{
	uint j;
	Matrix tmp1, tmp2;
	float det, res = 0.0f;

	tmp1 = create_dynamic_matrix_empty(mat.i - 1, mat.j);
	tmp2 = create_dynamic_matrix_empty(mat.i - 1, mat.j - 1);
	matrix_remove_row(tmp1, mat, 0);

	for (j = 0; j < mat.j; j++) {
		matrix_remove_column(tmp2, tmp1, j);
		det = determinant(tmp2);
		if (j % 2 == 0)
			res += mat.val[j] * det;
		else
			res -= mat.val[j] * det;
	}

	destroy_dynamic_matrix(tmp1);
	destroy_dynamic_matrix(tmp2);
}

float
determinant(Matrix mat)
{
#ifdef DEBUG
	assert(mat.val != NULL);
	assert(mat.i == mat.j);
#endif

	if (mat.i == 1)
		return mat.val[0];
	else if (mat.i == 2)
		return mat.val[0] * mat.val[3] - mat.val[1] * mat.val[2];
	else
		return det(mat);
}

void
transposed(Matrix res, Matrix mat)
{
	uint i, j;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.val != mat.val);
	assert(res.i == mat.j);
	assert(res.j == mat.i);
#endif

	for (i = 0; i < res.i; i++)
		for (j = 0; j < res.j; j++)
			res.val[i * res.j + j] = mat.val[j * mat.j + i];
}

void
transpose(Matrix res)
{
	Matrix tmp;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(res.i == res.j)
#endif

	tmp = create_dynamic_matrix_empty(res.i, res.j);

	transposed(tmp, res);
	matrix_copy(res, tmp);

	destroy_dynamic_matrix(tmp);
}

void
minors(Matrix res, Matrix mat)
{
	uint i;
	Matrix tmp1, tmp2;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
	assert(mat.i == mat.j);
	assert(mat.i > 1);
#endif

	tmp1 = create_dynamic_matrix_empty(mat.i - 1, mat.j);
	tmp2 = create_dynamic_matrix_empty(mat.i - 1, mat.j - 1);

	for (i = 0; i < mat.i * mat.j; i++) {
		matrix_remove_row(tmp1, mat,
		                  (uint)floor((float)i / (float)mat.j));
		matrix_remove_column(tmp2, tmp1, i % mat.j);
		res.val[i] = determinant(tmp2);
	}

	destroy_dynamic_matrix(tmp1);
	destroy_dynamic_matrix(tmp2);
}

void
checker_pattern(Matrix res, Matrix mat)
{
	uint i;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
	assert(mat.i > 1);
#endif

	for (i = 0; i < mat.i * mat.j; i++)
		if (i % 2 == (i / mat.j + 1) % 2)
			res.val[i] = -1 * mat.val[i];
}

void
cofactors(Matrix res, Matrix mat)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
	assert(mat.i == mat.j);
	assert(mat.i > 1);
#endif

	minors(res, mat);
	checker_pattern(res, res);
}

void
adjugate(Matrix res, Matrix mat)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.val != mat.val);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
	assert(mat.i == mat.j);
	assert(mat.i > 1);
#endif

	cofactors(res, mat);
	transpose(res);
}

float
determinant_from_cofactors(Matrix mat, Matrix cofactors)
{
	uint i;
	float res = 0.0f;

#ifdef DEBUG
	assert(mat.val != NULL);
	assert(cofactors.val != NULL);
	assert(mat.i == cofactors.i);
	assert(mat.j == cofactors.j);
	assert(mat.i == mat.j);
	assert(cofactors.i > 1);
#endif

	for (i = 0; i < mat.j; i++)
		res += cofactors.val[i] * mat.val[i];

	return res;
}

void
inverse(Matrix res, Matrix mat)
{
	Matrix c, a;
	float det;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(res.i == mat.j);
	assert(res.j == mat.i);
	assert(mat.i == mat.j);
#endif

	if (mat.i == 1) {
		res.val[0] = 1.0f / mat.val[0];
		return;
	}

	c = create_dynamic_matrix_empty(mat.i, mat.j);
	a = create_dynamic_matrix_empty(mat.i, mat.j);

	cofactors(c, mat);
	printf("\nCOFACTORS\n");
	print_matrix(c);
	transposed(a, c);
	printf("\nADJUGANT\n");
	print_matrix(a);
	det = determinant_from_cofactors(mat, c);
	printf("\nDETERMINANT\n");
	printf("%f\n", det);
	matrix_scalar_product(res, a, 1.0f / det);

	destroy_dynamic_matrix(c);
	destroy_dynamic_matrix(a);
}

void
homogeneous(Matrix res, Matrix mat)
{
	uint i, x = 0;

#ifdef DEBUG
	assert(mat.val != NULL);
	assert(res.val != NULL);
	assert(mat.i == mat.j);
	assert(res.i == res.j);
	assert(res.i == mat.i + 1)
#endif

	for (i = 0; i < mat.i * res.j; i++) {
		if (i % res.j == mat.j) {
			res.val[i] = 0.0f;
		} else {
			res.val[i] = mat.val[x];
			x++;
		}
	}

	for (; i < res.i * res.j - 1; i++)
		res.val[i] = 0.0f;

	res.val[res.i * res.j - 1] = 1.0f;
}

void
rotation_3d(Matrix res, Vector vec, float a)
{
	float cos_a, sin_a;
	Matrix tmp;

#ifdef DEBUG
	assert(vec.val != NULL);
	assert(res.val != NULL);
	assert(vec.i == 3);
	assert(res.i == 3);
	assert(res.j == 3);
#endif

	cos_a = cosf(a);
	sin_a = sinf(a);

	tmp = create_matrix(3, 3,
		/* row 1 */
		cos_a + vec.val[0] * vec.val[0] * (1.0f - cos_a),
		vec.val[0] * vec.val[1] * (1.0f - cos_a) - vec.val[2] * sin_a,
		vec.val[0] * vec.val[2] * (1.0f - cos_a) + vec.val[1] * sin_a,
		/* row 2 */
		vec.val[1] * vec.val[0] * (1.0f - cos_a) + vec.val[2] * sin_a,
		cos_a + vec.val[1] * vec.val[1] * (1.0f - cos_a),
		vec.val[1] * vec.val[2] * (1.0f - cos_a) - vec.val[0] * sin_a,
		/* row 3 */
		vec.val[2] * vec.val[0] * (1.0f - cos_a) - vec.val[1] * sin_a,
		vec.val[2] * vec.val[1] * (1.0f - cos_a) + vec.val[0] * sin_a,
		cos_a + vec.val[2] * vec.val[2] * (1.0f - cos_a),
	);

	matrix_copy(res, tmp);
}

void
rotation_3d_homogeneous(Matrix res, Vector vec, float a)
{
	Matrix tmp;

	tmp = create_matrix_empty(3, 3);

	rotation_3d(tmp, vec, a);
	homogeneous(res, tmp);
}

void
rotate_3d(Matrix res, Vector vec, float a)
{
	Matrix rot;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(res.i == 3);
	assert(res.j == 3);
#endif

	rot = create_matrix_empty(3, 3);

	rotation_3d(rot, vec, a);
	matrix_multiply_matrix_stack(3, rot, res);
}

void
rotate_3d_homogeneous(Matrix res, Vector vec, float a)
{
	Matrix rot;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(res.i == 4);
	assert(res.j == 4);
#endif

	rot = create_matrix_empty(4, 4);

	rotation_3d_homogeneous(rot, vec, a);
	matrix_multiply_matrix_stack(4, rot, res);
}

void
translate(Matrix res, Vector vec)
{
	uint i, x = 0;

#ifdef DEBUG
	assert(vec.val != NULL);
	assert(res.val != NULL);
	assert(res.i == res.j);
	assert(vec.i == mat.i - 1);
#endif

	for (i = res.j - 1; i < vec.i * res.j; i += res.j) {
		res.val[i] += vec.val[x];
		x++;
	}
}

void
translated(Matrix res, Matrix mat, Vector vec)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(mat.val != NULL);
	assert(vec.val != NULL);
	assert(mat.i == mat.j);
	assert(res.i == mat.i);
	assert(res.j == mat.j);
	assert(vec.i == mat.i - 1);
#endif

	matrix_copy(res, mat);
	translate(res, vec);
}

void
identity(Matrix res)
{
	uint x, end;

#ifdef DEBUG
	assert(res.val != NULL);
#endif

	end = res.i * res.j;

	for (x = 0; x < end; x++) {
		if (x % (res.j + 1) == 0)
			res.val[x] = 1.0f;
		else
			res.val[x] = 0.0f;
	}
}

static void
scalem(Matrix res, Vector vec, uint r)
{
	uint x, y = 0, step, end;

	step = res.j + 1;
	end = r * res.j;

	for (x = 0; x < end; x += step) {
		res.val[x] = vec.val[y];
		y++;
	}
}

void
scale_matrix(Matrix res, Vector vec)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == res.j);
	assert(vec.i == res.i);
#endif
	scalem(res, vec, res.i);
}

void
scale_matrix_homogeneous(Matrix res, Vector vec)
{
#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == res.j);
	assert(vec.i == res.i - 1);
#endif
	scalem(res, vec, res.i - 1);
}

void
scale(Matrix res, Vector vec)
{
	Matrix scale;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == res.j);
	assert(vec.i == res.i);
#endif

	scale = create_dynamic_matrix_empty(res.i, res.j);

	scale_matrix(scale, vec);
	matrix_multiply_matrix_dynamic(scale, res);

	destroy_dynamic_matrix(scale);
}

void
scale_homogeneous(Matrix res, Vector vec)
{
	Matrix scale;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(vec.val != NULL);
	assert(res.i == res.j);
	assert(vec.i == res.i - 1);
#endif

	scale = create_dynamic_matrix_empty(res.i, res.j);

	scale_matrix_homogeneous(scale, vec);
	matrix_multiply_matrix_dynamic(scale, res);

	destroy_dynamic_matrix(scale);
}

//bool
//scale_matrix_3d(Matrix *res, Vector vec)
//{
//	if (vec.i != 3 || !create_simple_matrix(res, 3, 3, 1.0f))
//		return false;
//
//	res->val[0] = vec.val[0];
//	res->val[4] = vec.val[1];
//	res->val[8] = vec.val[2];
//
//	return true;
//}
//
//bool
//scale_matrix_homogeneous(Matrix *res, Vector vec)
//{
//	if (vec.i != 3 || !create_simple_matrix(res, 4, 4, 1.0f))
//		return false;
//
//	res->val[0] = vec.val[0];
//	res->val[5] = vec.val[1];
//	res->val[10] = vec.val[2];
//
//	return true;
//}
//
//bool
//scale(Matrix *res, Matrix mat, Vector vec)
//{
//	Matrix scale;
//
//	if (mat.i == 3 && mat.j == 3) {
//		if (!scale_matrix_3d(&scale, vec))
//			return false;
//	} else if (mat.i == 4 && mat.j == 4) {
//		if (!scale_matrix_homogeneous(&scale, vec))
//			return false;
//	} else {
//		return false;
//	}
//
//	if (!matrix_matrix_product(res, mat, scale))
//		return false;
//
//	return true;
//}

void
print_vector(Vector vec)
{
	uint i;

	for (i = 0; i < vec.i; i++)
		printf("%f\n", vec.val[i]);
}

void
print_matrix(Matrix mat)
{
	uint i, j;

	for (i = 0; i < mat.i; i++) {
		for (j = 0; j < mat.j; j++)
			printf("%f ", mat.val[i * mat.j + j]);
		printf("\n");
	}
}
