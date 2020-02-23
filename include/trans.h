/*
 * TODO:
 * vector matrix product
 * sums
 * error messages
 */

#ifndef TRANS_H
#define TRANS_H

#include <stdbool.h>

/* data types */
typedef struct {
	unsigned int i;
	float *val;
} vector;

typedef struct {
	unsigned int i;
	unsigned int j;
	float *val;
} matrix;

/* function declarations */
bool create_vector(vector *vec, unsigned int i);
void vector_copy_data(vector vec, const float *data);
bool vector_scalar_product(vector *res, vector vec, float s);
bool vector_vector_product(float *res, vector vec1, vector vec2);
bool normalize_vector(vector *res, vector vec);
bool create_matrix(matrix *res, unsigned int i, unsigned int j);
bool create_simple_matrix(matrix *res, unsigned int i, unsigned int j, float s);
void matrix_copy_data(matrix mat, const float *data);
bool copy_matrix(matrix *res, matrix mat);
bool vector_from_matrix(vector *res, matrix mat, unsigned int i);
bool matrix_remove_row(matrix *res, matrix mat, unsigned int i);
bool matrix_remove_column(matrix *res, matrix mat, unsigned int j);
bool matrix_scalar_product(matrix *res, matrix mat, float s);
bool matrix_vector_product(vector *res, matrix mat, vector vec);
bool matrix_matrix_product(matrix *res, matrix mat1, matrix mat2);
bool transpose(matrix *res, matrix mat);
bool determinant(float *res, matrix mat);
bool matrix_of_minors(matrix *res, matrix mat);
bool inverse_matrix(matrix *res, matrix mat);
bool rotate(matrix *res, matrix mat, float a, vector vec);
bool matrix_from_axis_angle(matrix *res, float a, vector vec);
bool translate(matrix *res, matrix mat, vector vec);
bool scale_matrix_3d(matrix *res, vector vec);
bool scale_matrix_homogeneous(matrix *res, vector vec);
bool scale(matrix *res, matrix mat, vector vec);
void print_vector(vector vec);
void print_matrix(matrix mat);

#endif
