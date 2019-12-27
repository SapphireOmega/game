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
bool create_vector(vector *vec, int i);
void copy_vector_data(vector vec, const float *data);
bool vector_scalar_product(vector *res, vector vec, float s);
bool vector_vector_product(float *res, vector vec1, vector vec2);
bool normalize_vector(vector *res, vector vec);
bool create_matrix(matrix *res, int i, int j);
bool create_simple_matrix(matrix *res, int i, int j, float s);
void copy_matrix_data(matrix mat, const float *data);
bool vector_from_matrix(vector *res, matrix mat, int i);
bool matrix_scalar_product(matrix *res, matrix mat, float s);
bool matrix_vector_product(vector *res, matrix mat, vector vec);
bool matrix_matrix_product(matrix *res, matrix mat1, matrix mat2);
bool transpose(matrix *res, matrix mat);
float determinant(matrix mat);
bool rotate(matrix *res, matrix mat, float a, vector vec);
bool matrix_from_axis_angle(matrix *res, float a, vector vec);
void print_vector(vector vec);
void print_matrix(matrix mat);

#endif
