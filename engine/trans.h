/*
 * Functions ending with _unsafe should not be used since they omit the error
 * checking. Use the macro's of the same name but without the suffix _unsafe
 * instead.
 */

#ifndef TRANS_H
#define TRANS_H

/*
 * TODO:
 * get vector from matrix column
 * function for creating identity matrices on stack
 * matrix add row/column
 * sums
 * proper error handling instead of asserts everywhere
 */

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "engine.h"

#define create_vector_empty(i)                                                 \
        (Vector){ i, (float[i]){} }
#define create_matrix_empty(i, j)                                              \
        (Matrix){ i, j, (float[i * j]){} }

#ifdef DEBUG

#define create_vector(i, ...)                                                  \
        (Vector){ i, (float[i]){ __VA_ARGS__ } };                              \
        assert(i == LEN((float[]){ __VA_ARGS__ }))
#define create_matrix(i, j, ...)                                               \
        (Matrix){ i, j, (float[i * j]){ __VA_ARGS__ } };                       \
        assert(i * j == LEN((float[]){__VA_ARGS__}))
#define create_dynamic_vector(i, ...)                                          \
        create_dynamic_vector_unsafe(i, __VA_ARGS__);                          \
        assert(i == LEN((float[]){ __VA_ARGS__ }))
#define create_dynamic_matrix(i, j, ...)                                       \
        create_dynamic_matrix_unsafe(i, j, __VA_ARGS__);                       \
        assert(i * j == LEN((float[]){ __VA_ARGS__ }))
#define matrix_multiply_vector_stack(i, mat, res)                              \
        {                                                                      \
                assert(res.val != NULL);                                       \
                assert(mat.val != NULL);                                       \
                assert(res.i == i);                                            \
                assert(res.j == i);                                            \
                assert(vec.i == i);                                            \
                Vector tmp = create_vector_empty(i);                           \
                matrix_vector_product(tmp, mat, res);                          \
                vector_copy(res, tmp);                                         \
        }
#define matrix_multiply_matrix_stack(i, mat, res)                              \
        {                                                                      \
                assert(res.val != NULL);                                       \
                assert(mat.val != NULL);                                       \
                assert(res.i == i);                                            \
                assert(res.j == i);                                            \
                assert(mat.i == i);                                            \
                assert(mat.j == i);                                            \
                Matrix tmp = create_matrix_empty(i, i);                        \
                matrix_matrix_product(tmp, mat, res);                          \
                matrix_copy(res, tmp);                                         \
        }

#else

#define create_vector(i, ...)                                                  \
        (Vector){ i, (float[i]){ __VA_ARGS__ } };
#define create_matrix(i, j, ...)                                               \
        (Matrix){ i, j, (float[i * j]){ __VA_ARGS__ } };
#define create_dynamic_vector(i, ...)                                          \
        create_dynamic_vector_unsafe(i, __VA_ARGS__)
#define create_dynamic_matrix(i, j, ...)                                       \
        create_dynamic_matrix_unsafe(i, j, __VA_ARGS__)
#define matrix_multiply_vector_stack(i, mat, res)                              \
        {                                                                      \
                Vector tmp = create_vector_empty(i);                           \
                matrix_vector_product(tmp, mat, res);                          \
                vector_copy(res, tmp);                                         \
        }
#define matrix_multiply_matrix_stack(i, mat, res)                              \
        {                                                                      \
                Matrix tmp = create_matrix_empty(i, i);                        \
                matrix_matrix_product(tmp, mat, res);                          \
                matrix_copy(res, tmp);                                         \
        }

#endif

/* types */
typedef struct {
	uint i;
	float *val;
} Vector;

typedef struct {
	uint i;
	uint j;
	float *val;
} Matrix;

/* functions */
Vector create_dynamic_vector_unsafe(uint i, ...);
Matrix create_dynamic_matrix_unsafe(uint i, uint j, ...);
Vector create_dynamic_vector_empty(uint i);
Matrix create_dynamic_matrix_empty(uint i, uint j);

static inline void destroy_dynamic_vector(Vector vec) { free(vec.val); };
static inline void destroy_dynamic_matrix(Matrix mat) { free(mat.val); };

void vector_copy(Vector res, Vector vec);
void matrix_copy(Matrix res, Matrix mat);

void normalized_vector(Vector res, Vector vec);
/* Same function as above, but modifies the vector directly */
static inline void normalize_vector(Vector res)
{ normalized_vector(res, res); }

void vector_scalar_product(Vector res, Vector vec, float s);
float vector_vector_product(Vector vec1, Vector vec2);
void matrix_scalar_product(Matrix res, Matrix mat, float s);
void matrix_vector_product(Vector res, Matrix mat, Vector vec);
void matrix_matrix_product(Matrix res, Matrix m1, Matrix m2);

/* These functions are the same as above, but modify the arguments directly */
static inline void vector_multiply_scalar(Vector res, float s)
{ vector_scalar_product(res, res, s); }
static inline void matrix_multiply_scalar(Matrix res, float s)
{ matrix_scalar_product(res, res, s); }
void matrix_multiply_vector_dynamic(Matrix mat, Vector res);
void matrix_multiply_matrix_dynamic(Matrix mat, Matrix res);

void vector_from_matrix(Vector res, Matrix mat, uint i);
void matrix_remove_row(Matrix res, Matrix mat, uint i);
void matrix_remove_column(Matrix res, Matrix mat, uint j);

float determinant(Matrix mat);
void transposed(Matrix res, Matrix mat);
/* Same function as above, but modifies the matrix directly */
void transpose(Matrix res);
void minors(Matrix res, Matrix mat);
void checker_pattern(Matrix res, Matrix mat);
void cofactors(Matrix res, Matrix mat);
void adjugate(Matrix res, Matrix mat);
float determinant_from_cofactors(Matrix mat, Matrix cofactors);
void inverse(Matrix res, Matrix mat);
/* Same function as above, but modifies the matrix directly */
static inline void invert(Matrix res) { inverse(res, res); }

void homogeneous(Matrix res, Matrix mat);

void rotation_3d(Matrix res, Vector vec, float a);
void rotation_homogeneous_3d(Matrix res, Vector vec, float a);
/* Same functions as above, but multiply existing matrices with the rotations */
void rotate_3d(Matrix res, Vector vec, float a);
void rotate_3d_homogeneous(Matrix res, Vector vec, float a);

void translate(Matrix res, Vector vec);
void translated(Matrix res, Matrix mat, Vector vec);

void identity(Matrix res);

void scale_matrix(Matrix res, Vector vec);
void scale_matrix_homogeneous(Matrix res, Vector vec);
/* Same as above, but multiply existing matrices with the scale matrices */
void scale(Matrix res, Vector vec);
void scale_homogeneous(Matrix res, Vector vec);

void print_vector(Vector vec);
void print_matrix(Matrix mat);

#endif
