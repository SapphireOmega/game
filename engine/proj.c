#include "proj.h"

#include <math.h>

/* globals */
static Matrix viewm;
static Matrix projm;

/* functions */
void
fps_view(Matrix res)
{
	Matrix tmp;
	Vector axis;

#ifndef DEBUG
	assert(res.val != NULL);
	assert(res.i == 4);
	assert(res.j == 4);
#endif

	tmp = create_matrix(4, 4,
		1.0f, 0.0f, 0.0f, current_camera->x,
		0.0f, 1.0f, 0.0f, current_camera->y,
		0.0f, 0.0f, 1.0f, current_camera->z,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	matrix_copy(res, tmp);
	axis = create_vector(3, 0.0f, 1.0f, 0.0f);
	rotate_3d_homogeneous(res, axis, current_camera->angle_y);
	axis = create_vector(3, 1.0f, 0.0f, 0.0f);
	rotate_3d_homogeneous(res, axis, current_camera->angle_x);
	invert(res);
}

void
perspective(Matrix res, float aspect)
{
	float t, b, l, r, n, f;
	Matrix tmp;

#ifdef DEBUG
	assert(res.val != NULL);
	assert(res.i == 4);
	assert(res.j == 4);
#endif

	n = current_camera->n;
	f = current_camera->f;
	t = tanf(current_camera->fovx / 2.0f) * n;
	b = -t;
	r = t * aspect;
	l = -r;

	tmp = create_matrix(4, 4,
		2.0f * n / (r - l), 0.0f, (l + r) / (r - l), 0.0f,
		0.0f, 2.0f * n / (t - b), (b + t) / (t - b), 0.0f,
		0.0f, 0.0f, -(f + n) / (f - n), -2.0f * f * n / (f - n),
		0.0f, 0.0f, -1.0f, 0.0f
	);

	matrix_copy(res, tmp);
}
