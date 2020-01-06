#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include "engine/engine.h"
#include "engine/engine_time.h"
#include "engine/event.h"
#include "engine/shader.h"
#include "engine/state.h"
#include "engine/imgload.h"
#include "engine/proj.h"
#include "engine/trans.h"
#include "engine/util.h"
#include "engine/window.h"

/* function declarations */
static void move_foreward(void);
static void move_backward(void);
static void move_left(void);
static void move_right(void);
static void rot_left(void);
static void rot_right(void);

static void setup(void);
static void render(void);
static void cleanup(void);

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

vector vel;

/* function definitions */
void
move_foreward(void)
{
	vector tmp;

	if (!create_vector(&tmp, 3))
		die("error creating vector\n");

	tmp.val[0] = -50.0f * cosf(M_PI_2 - cam.angle_y);
	tmp.val[2] = -50.0f * cosf(cam.angle_y);

	vel.val[0] += tmp.val[0];
	vel.val[2] += tmp.val[2];
}

void
move_backward(void)
{
	vector tmp;

	if (!create_vector(&tmp, 3))
		die("error creating vector\n");

	tmp.val[0] = 50.0f * cosf(M_PI_2 - cam.angle_y);
	tmp.val[2] = 50.0f * cosf(cam.angle_y);

	vel.val[0] += tmp.val[0];
	vel.val[2] += tmp.val[2];
}

void
move_left(void)
{
	vector tmp;

	if (!create_vector(&tmp, 3))
		die("error creating vector\n");

	tmp.val[0] = -50.0f * cosf(cam.angle_y);
	tmp.val[2] = 50.0f * cosf(M_PI_2 - cam.angle_y);

	vel.val[0] += tmp.val[0];
	vel.val[2] += tmp.val[2];
}

void
move_right(void)
{
	vector tmp;

	if (!create_vector(&tmp, 3))
		die("error creating vector\n");

	tmp.val[0] = 50.0f * cosf(cam.angle_y);
	tmp.val[2] = -50.0f * cosf(M_PI_2 - cam.angle_y);

	vel.val[0] += tmp.val[0];
	vel.val[2] += tmp.val[2];
}

void
rot_left(void)
{
	cam.angle_y += 50.0f * (float)delta_time;
}

void
rot_right(void)
{
	cam.angle_y -= 50.0f * (float)delta_time;
}

void
setup(void)
{
	GLint pos_attrib, col_attrib, tex_attrib, status;
	char *vs_src, *fs_src, *err;

	engine_create_window(800, 600);
	XAutoRepeatOff(display);

	printf("creating vertex array\n");
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	printf("creating vertex buffer\n");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	             GL_STATIC_DRAW);

	printf("creating element buffer\n");
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
	             GL_STATIC_DRAW);

	printf("creating shader program\n");
	parse_shader("res/shaders/shader.glsl", &vs_src, &fs_src);
	shader_program = create_shader_program(vs_src, fs_src);
	glUseProgram(shader_program);
	printf("shader program created\n");

	pos_attrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE,
	                      7 * sizeof(float), 0);

	col_attrib = glGetAttribLocation(shader_program, "vcolor");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE,
	                      7 * sizeof(float), (void *)(2 * sizeof(float)));

	tex_attrib = glGetAttribLocation(shader_program, "vtexcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE,
	                      7 * sizeof(float), (void *)((5 * sizeof(float))));

	if (!load_tga_file(&test_image, "res/textures/test.tga"))
		die("error loading tga file: %s\n", img_strerror(img_err));

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	             test_image.width, test_image.height,
		     0, GL_RGBA, GL_FLOAT, test_image.data);

	if (!add_key(err, XK_w, NULL, NULL, move_foreward))
		die("error adding key: %s", err);
	if (!add_key(err, XK_s, NULL, NULL, move_backward))
		die("error adding key: %s", err);
	if (!add_key(err, XK_a, NULL, NULL, move_left))
		die("error adding key: %s", err);
	if (!add_key(err, XK_d, NULL, NULL, move_right))
		die("error adding key: %s", err);
	if (!add_key(err, XK_q, NULL, NULL, rot_left))
		die("error adding key: %s", err);
	if (!add_key(err, XK_e, NULL, NULL, rot_right))
		die("error adding key: %s", err);

	if (!create_vector(&vel, 3))
		die("error creating velocity vector\n");

	current_camera = &cam;
}

void
render(void)
{
	matrix proj, model, camm, viewm;
	matrix rot1, cam_rot;
	vector axis1, cam_axis, tmp;
	GLint proj_uni, model_uni, view_uni;
	unsigned int width, height;
	float aspect;
	unsigned int dummy;

	if (!normalize_vector(&tmp, vel))
		die("error normalizing vector\n");
	if (!vector_scalar_product(&tmp, tmp, 50.0f))
		die("error multiplying vector and scalar\n");

	cam.x += tmp.val[0] * (float)delta_time;
	cam.y += tmp.val[1] * (float)delta_time;
	cam.z += tmp.val[2] * (float)delta_time;

	vel.val[0] = vel.val[1] = vel.val[2] = 0.0f;

	//float raxis1[] = { 1.0f, 1.0f, 0.0f };
	//if (!create_vector(&axis1, 3))
	//	die("error creating vector axis1\n");
	//vector_copy_data(axis1, raxis1);
	//if (!normalize_vector(&axis1, axis1))
	//	die("error normalizing vector axis1\n");
	if (!create_simple_matrix(&rot1, 4, 4, 1.0f))
		die("error creating matrix\n");
	//if (!rotate(&rot1, rot1, 0.0f, axis1))
	//	die("error rotating matrix rot1\n");
	model = rot1;

	if (!view(&viewm))
		die("error getting view matrix");

	aspect = (float)window_attribs.width / (float)window_attribs.height;
	if (!perspective(&proj, aspect))
		die("error getting perspective");

	/* OpenGl uses column-major order (I found out the hard way) */
	if (!transpose(&model, model))
		die("error transposing model matrix\n");
	if (!transpose(&viewm, viewm))
		die("error transposing view matrix\n");
	if (!transpose(&proj, proj))
		die("error transposing projection matrix\n");

	model_uni = glGetUniformLocation(shader_program, "model");
	glUniformMatrix4fv(model_uni, 1, GL_FALSE, model.val);

	view_uni = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(view_uni, 1, GL_FALSE, viewm.val);

	proj_uni = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(proj_uni, 1, GL_FALSE, proj.val);

	glClearColor(0.0, 0.7, 0.7, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, sizeof(elements) / sizeof(GLuint),
	               GL_UNSIGNED_INT, 0);

	glXSwapBuffers(display, window);
}

void
cleanup(void)
{
	glDeleteProgram(shader_program);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

int
main(int argc, char *argv[])
{
	GameState game_state = { setup, NULL, render, cleanup };

	engine_set_current_state(game_state);
	engine_run();

	return EXIT_SUCCESS;
}
