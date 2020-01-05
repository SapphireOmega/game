#include "proj.h"

#include <math.h>
#include <stdio.h>

bool
view(matrix *res)
{
	vector axis;

	const float rres[] = {
		1.0f, 0.0f, 0.0f, current_camera->x,
		0.0f, 1.0f, 0.0f, current_camera->y,
		0.0f, 0.0f, 1.0f, current_camera->z,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	const float raxis[] = { 1.0f, 0.0f, 0.0f };

	if (!create_matrix(res, 4, 4))
		return false;
	matrix_copy_data(*res, rres);
	if (!create_vector(&axis, 3))
		return false;
	vector_copy_data(axis, raxis);
	if (!rotate(res, *res, current_camera->angle_x, axis))
		return false;
	axis.val[0] = 0.0f;
	axis.val[1] = 1.0f;
	if (!rotate(res, *res, current_camera->angle_y, axis))
		return false;
	axis.val[1] = 0.0f;
	axis.val[2] = 1.0f;
	if (!rotate(res, *res, current_camera->angle_z, axis))
		return false;
	if (!inverse_matrix(res, *res))
		return false;

	return true;
}

bool
perspective(matrix *res, float aspect)
{
	float t, b, l, r, n, f;

	n = current_camera->n;
	f = current_camera->f;
	t = tanf(current_camera->fovx / 2.0f) * n;
	b = -t;
	r = t * aspect;
	l = -r;

	float rres[] = {
		2.0f * n / (r - l), 0.0f, (l + r) / (r - l), 0.0f,
		0.0f, 2.0f * n / (t - b), (b + t) / (t - b), 0.0f,
		0.0f, 0.0f, -(f + n) / (f - n), -2.0f * f * n / (f - n),
		0.0f, 0.0f, -1.0f, 0.0f
	};

	if (!create_matrix(res, 4, 4))
		return false;
	matrix_copy_data(*res, rres);

	return true;
}
