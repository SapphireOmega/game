#include "proj.h"

#include <math.h>
#include <stdio.h>

/* globals */
static Matrix viewm;
static Matrix projm;

/* functions */
void
fps_view(Matrix res)
{
	Vector vec;
	Matrix rot;

#ifndef DEBUG
	assert(res.val != NULL);
	assert(res.i == 4);
	assert(res.j == 4);
#endif

	rot = create_matrix_empty(3, 3);
	vec = create_vector(3, 0.0f, 1.0f, 0.0f);
	rotation_3d(rot, vec, current_camera->angle_y);
	vec = create_vector(3, 1.0f, 0.0f, 0.0f);
	matrix_multiply_vector_stack(3, rot, vec);
	rotate_3d(rot, vec, current_camera->angle_x);
	homogeneous(res, rot);
	vec = create_vector(3,
		current_camera->x,
		current_camera->y,
		current_camera->z
	);
	translate(res, vec);
	invert(res);
}

void
projection(Matrix res, float aspect)
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

	if (current_camera->proj == PERSP) {
		tmp = create_matrix(4, 4,
			2.0f * n / (r - l), 0.0f, (l + r) / (r - l), 0.0f,
			0.0f, 2.0f * n / (t - b), (b + t) / (t - b), 0.0f,
			0.0f, 0.0f, -(f + n) / (f - n), -2.0f * f * n / (f - n),
			0.0f, 0.0f, -1.0f, 0.0f
		);
	} else if (current_camera->proj == ORTHO) {
		tmp = create_matrix(4, 4,
			2.0f / (r - l), 0.0f, 0.0f, - (r + l) / (r - l),
			0.0f, 2.0f / (t - b), 0.0f, - (t + b) / (t - b),
			0.0f, 0.0f, -2.0f / (f - n), - (f + n) / (f - n),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	matrix_copy(res, tmp);
}
