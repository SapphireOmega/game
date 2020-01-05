#ifndef GAME_H
#define GAME_H

#include <GL/glew.h>

#include "engine/imgload.h"
#include "engine/proj.h"

/* function declarations */
void move_foreward(void);
void move_backward(void);
void move_left(void);
void move_right(void);
void rot_left(void);
void rot_right(void);

void setup(void);
void render(void);
void cleanup(void);

/* globals */
static GLuint shader_program;
static GLuint vao;
static GLuint vbo;
static GLuint ebo;
static struct tga_file test_image;
static GLuint tex;
static struct camera cam = {
	.x = 0.0f, .y = 0.0f, .z = 1.0f,
	.angle_x = 0.0f, .angle_y = 0.0f, .angle_z = 0.0f,
	.fovx = 1.570796f,
	.proj = PERSP,
	.n = 0.1f, .f = 100.0f
};

static const float vertices[] = {
/*      pos           color             texcoords */
	-0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
};

static const GLuint elements[] = {
	0, 1, 2,
	2, 3, 0,
};

#endif /* GAME_H */
