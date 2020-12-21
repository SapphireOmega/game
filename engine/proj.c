#include "proj.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "vec.h"

void fps_view(float res[4][4])
{
	float x[3] = { 1.0f, 0.0f, 0.0f };
	float y[3] = { 0.0f, 1.0f, 0.0f };
	float x_rel[3];
	float cam_pos[3] = { current_camera->x, current_camera->y, current_camera->z };

	float Ry[3][3];
	float Rx_rel[3][3];
	float Rtot[3][3];
	float Rtot_h[4][4];
	float transl[4][4];

	rotation_3d(Ry, y, current_camera->angle_y);

	matrix_vector_product(3, 3, x_rel, Ry, x);
	rotation_3d(Rx_rel, x_rel, current_camera->angle_x);
	matrix_matrix_product(3, 3, 3, Rtot, Rx_rel, Ry);
	homogeneous(3, Rtot_h, Rtot);
	translation(3, transl, cam_pos);
	matrix_matrix_product(4, 4, 4, res, transl, Rtot_h);
	inverse(4, res, res);
}

void projection(float res[4][4], float aspect)
{
	float n = current_camera->n;
	float f = current_camera->f;
	float t = tanf(current_camera->fovx / 2.0f) * n;
	float b = -t;
	float r = t * aspect;
	float l = -r;

	switch (current_camera->proj) {
	case PERSP:
		res[0][0] = 2.0f * n / (r - l);
		res[0][1] = 0.0f;
		res[0][2] = (l + r) / (r - l);
		res[0][3] = 0.0f;

		res[1][0] = 0.0f;
		res[1][1] = 2.0f * n / (t - b);
		res[1][2] = (b + t) / (t - b);
		res[1][3] = 0.0f;

		res[2][0] = 0.0f;
		res[2][1] = 0.0f;
		res[2][2] = -(f + n) / (f - n);
		res[2][3] = -2.0f * f * n / (f - n);

		res[3][0] = 0.0f;
		res[3][1] = 0.0f;
		res[3][2] = -1.0f;
		res[3][3] = 0.0f;
		break;
	case ORTHO:
		res[0][0] = 2.0f / (r - l);
		res[0][1] = 0.0f;
		res[0][2] = 0.0f;
		res[0][3] = -(r + l) / (r - l);

		res[1][0] = 0.0f;
		res[1][1] = 2.0f / (t - b);
		res[1][2] = 0.0f;
		res[1][3] = -(t + b) / (t - b);

		res[2][0] = 0.0f;
		res[2][1] = 0.0f;
		res[2][2] = -2.0f / (f - n);
		res[2][3] = -(f + n) / (f - n);

		res[3][0] = 0.0f;
		res[3][1] = 0.0f;
		res[3][2] = 0.0f;
		res[3][3] = 1.0f;
		break;
	}
}
