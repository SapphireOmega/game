#ifndef PROJ_H
#define PROJ_H

#include "trans.h"

#include <stdbool.h>

/* enums */
enum proj_type { PERSP, ORTHO };

/* structs */
struct camera {
	float x, y, z;
	float angle_x, angle_y, angle_z;
	float fovx;
	enum proj_type proj;
	float n, f;
};

/* variables */
struct camera *current_camera;

/* functions */
bool fps_view(matrix *res);
bool perspective(matrix *res, float aspect);

#endif
