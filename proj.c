#include "proj.h"

#include <math.h>
#include <stdio.h>

bool perspective(matrix *res, float fovx, float aspect, float n, float f)
{
	float t, b, l, r;

	//r = tanf(fovx / 2.0f) * n;
	//l = -r;
	//t = r / aspect;
	//b = -t;
	t = tanf(fovx / 2.0f) * n;
	b = -t;
	r = t * aspect;
	l = -r;

	//printf("aspect: %f, fovx: %f, n: %f, f: %f, l: %f, r: %f, t: %f, b: %f\n",
	//	aspect, fovx, n, f, l, r, t, b);

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
