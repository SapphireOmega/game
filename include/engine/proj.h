#ifndef PROJ_H
#define PROJ_H

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
//void fps_view(Matrix res);
void fps_view(float res[4][4]);
//void projection(Matrix res, float aspect);
void projection(float res[4][4], float aspect);

#endif
