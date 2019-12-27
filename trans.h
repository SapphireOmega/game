/*
 * --- TODO ---
 * change unnecessary pointers to plain structures
 * function that returns rotation matrices
 * matrix scalar dot product
 */

#ifndef TRANS_H
#define TRANS_H

#include <stdbool.h>

/* data types */
typedef struct {
	int i;
	float *val;
} vector;

typedef struct {
	int i;
	int j;
	float *val;
} matrix;

/* function declarations */
vector *create_vector(int i);
void copy_vector_data(vector *vec, const float *data);
vector *vector_scalar_product(const vector *vec, float s);
bool vector_vector_product(float *res, const vector *vec1, const vector *vec2);
vector *normalize_vector(const vector *vec);
matrix *create_matrix(int i, int j);
matrix *create_simple_matrix(int i, int j, float s);
void copy_matrix_data(matrix *mat, const float *data);
vector *vector_from_matrix(const matrix *mat, int i);
matrix *matrix_scalar_product(const matrix *mat, float s);
vector *matrix_vector_product(const matrix *mat, const vector *vec);
matrix *matrix_matrix_product(const matrix *mat1, const matrix *mat2);
matrix *rotate(const matrix *mat, float a, const vector *vec);
matrix *matrix_from_axis_angle(float a, const vector *vec);
void print_vector(const vector *vec);
void print_matrix(const matrix *mat);

#endif
