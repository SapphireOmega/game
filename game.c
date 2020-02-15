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
static void rot(float a, float b);
static void mouse_move(MouseMove m);

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
	.n = 0.01f, .f = 100.0f
};

//static const float vertices[] = {
///*      pos                  color             texcoords */
//	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//
//	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//};

static const float vertices[] = {
/*      pos                  color             texcoords */
	/* front */
	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,

	/* back */
	 0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,

	/* left */
	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,

	/* right */
	 0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,

	 /* top */
	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
 	
	/* bottom */
	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
};

// 5, 4, 7, 7, 6, 5

//static const GLuint elements[] = {
//	/* front */
//	0, 1, 2,
//	2, 3, 0,
//	/* back */
//	4, 5, 6,
//	6, 7, 4,
//	/* left */
//	4, 0, 3,
//	3, 7, 4,
//	/* right */
//	1, 5, 6,
//	6, 2, 1,
//	/* top */
//	4, 5, 1,
//	1, 0, 4,
//	/* bottom */
//	3, 2, 6,
//	6, 7, 3
//};

vector vel, quad_axis;
matrix quad_rot;

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

//void
//rot_left(void)
//{
//	cam.angle_y += 50.0f * (float)delta_time;
//}
//
//void
//rot_right(void)
//{
//	cam.angle_y -= 50.0f * (float)delta_time;
//}

void
rot(float a, float b)
{
	cam.angle_y += a * (float)delta_time;
	cam.angle_x += b * (float)delta_time;
	if (cam.angle_x > M_PI / 2.0f)
		cam.angle_x = M_PI / 2.0f;
	else if (cam.angle_x < -M_PI / 2.0f)
		cam.angle_x = -M_PI / 2.0f;
}

void
mouse_move(MouseMove m)
{
	rot((float)m.x * -1.0f, (float)m.y * -1.0f);
}

void
left(void)
{
	rot(10.0f, 0.0f);
}

void
right(void)
{
	rot(-10.0f, 0.0f);
}

void
up(void)
{
	rot(0.0f, 10.0f);
}

void
down(void)
{
	rot(0.0f, -10.0f);
}

void
setup(void)
{
	GLint pos_attrib, col_attrib, tex_attrib, status;
	char *vs_src, *fs_src, *err;
	float rquad_axis[] = { 0.0f, 0.0f, 1.0f };

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

	//printf("creating element buffer\n");
	//glGenBuffers(1, &ebo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
	//             GL_STATIC_DRAW);

	printf("creating shader program\n");
	parse_shader("res/shaders/shader.glsl", &vs_src, &fs_src);
	shader_program = create_shader_program(vs_src, fs_src);
	glUseProgram(shader_program);
	printf("shader program created\n");

	pos_attrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 3, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), 0);

	col_attrib = glGetAttribLocation(shader_program, "vcolor");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)(3 * sizeof(float)));

	tex_attrib = glGetAttribLocation(shader_program, "vtexcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE,
	                      8 * sizeof(float), (void *)((6 * sizeof(float))));

	if (!load_tga_file(&test_image, "res/textures/test.tga"))
		die("error loading tga file: %s\n", img_strerror(img_err));

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	             test_image.width, test_image.height,
		     0, GL_RGBA, GL_FLOAT, test_image.data);

	glEnable(GL_DEPTH_TEST);

	if (!add_key(err, XK_w, NULL, NULL, move_foreward))
		die("error adding key: %s", err);
	if (!add_key(err, XK_s, NULL, NULL, move_backward))
		die("error adding key: %s", err);
	if (!add_key(err, XK_a, NULL, NULL, move_left))
		die("error adding key: %s", err);
	if (!add_key(err, XK_d, NULL, NULL, move_right))
		die("error adding key: %s", err);
	//if (!add_key(err, XK_q, NULL, NULL, left))
	//	die("error adding key: %s", err);
	//if (!add_key(err, XK_e, NULL, NULL, right))
	//	die("error adding key: %s", err);
	//if (!add_key(err, XK_r, NULL, NULL,up))
	//	die("error adding key: %s", err);
	//if (!add_key(err, XK_f, NULL, NULL,down))
	//	die("error adding key: %s", err);
	mouse_handler.move = mouse_move;

	if (!create_vector(&vel, 3))
		die("error creating velocity vector\n");

	current_camera = &cam;

	if (!create_vector(&quad_axis, 3))
		die("error creating vector quad_axis\n");
	vector_copy_data(quad_axis, rquad_axis);
	if (!normalize_vector(&quad_axis, quad_axis))
		die("error normalizing vector quad_axis\n");

	if (!create_simple_matrix(&quad_rot, 4, 4, 1.0f))
		die("error creating matrix\n");
}

void
update(void)
{
	vector tmp;

	if (!normalize_vector(&tmp, vel))
		die("error normalizing vector\n");
	if (!vector_scalar_product(&tmp, tmp, 50.0f))
		die("error multiplying vector and scalar\n");

	cam.x += tmp.val[0] * (float)delta_time;
	cam.y += tmp.val[1] * (float)delta_time;
	cam.z += tmp.val[2] * (float)delta_time;

	vel.val[0] = vel.val[1] = vel.val[2] = 0.0f;

	//if (!rotate(&quad_rot, quad_rot, 20.0f * delta_time, quad_axis))
	//	die("error applying rotation to quad_rot\n");
}

void
render(void)
{
	matrix proj, model, viewm;
	GLint proj_uni, model_uni, view_uni;
	unsigned int width, height;
	float aspect;

	if (!fps_view(&viewm))
		die("error getting view matrix");

	aspect = (float)window_attribs.width / (float)window_attribs.height;
	if (!perspective(&proj, aspect))
		die("error getting perspective");

	model = quad_rot;

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDrawElements(GL_TRIANGLES, sizeof(elements) / sizeof(GLuint),
	//               GL_UNSIGNED_INT, 0);
	
	glDrawArrays(GL_TRIANGLES, 0, 36);

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
	GameState game_state = { setup, update, render, cleanup };

	engine_set_current_state(game_state);
	engine_run();

	return EXIT_SUCCESS;
}
