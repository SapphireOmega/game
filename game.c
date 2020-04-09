/* TODO:
 * clean up vertecies and matrices
 * error checking in the functions instead of by the program
 * fix the X connection broken message
 * use shorthand types and sort out opengl datatypes vs regular c
 * check if everything is being cleaned up
 * shader abstraction
 * texture abstraction
 * mipmaps
 * opengl error logging
 * gamestate stack
 * collision
 * sound
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>

#include <engine/engine.h>
#include <engine/engine_time.h>
#include <engine/event.h>
#include <engine/shader.h>
#include <engine/state.h>
#include <engine/imgload.h>
#include <engine/proj.h>
#include <engine/trans.h>
#include <engine/util.h>
#include <engine/window.h>

/* globals */
static GLuint shader_program;
static VertexBuffer *vb;
static VertexBufferLayout *vb_layout;
static VertexArray *va;
static struct TGA_File test_image;
static GLuint tex;
static struct Camera cam = {
	.x = 0.0f, .y = 0.0f, .z = 1.0f,
	.angle_x = 0.0f, .angle_y = 0.0f, .angle_z = 0.0f,
	.fovx = 1.570796f,
	.proj = PERSP,
	.n = 0.05f, .f = 100.0f
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

	/* floor */
	-1.0f, -0.5001f, -1.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f,
	 1.0f, -0.5001f, -1.0f, 0.5f, 0.0f, 0.5f, 0.0f, 0.0f,
	 1.0f, -0.5001f,  1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	 1.0f, -0.5001f,  1.0f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f,
	-1.0f, -0.5001f,  1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 0.0f,
	-1.0f, -0.5001f, -1.0f, 1.0f, 0.0f, 0.5f, 0.0f, 0.0f,
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

/* functions */
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
move_up(void)
{
	vel.val[1] += 50.0f;
}

void
move_down(void)
{
	vel.val[1] -= 50.0f;
}

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
mouse_move(struct MouseMove m)
{
	rot((float)m.x * -0.5f, (float)m.y * -0.5f);
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
	char *vs_src, *fs_src, *err;
	float rquad_axis[] = { 0.0f, 0.0f, 1.0f };

	engine_create_window(800, 600);
	XAutoRepeatOff(display);

	parse_shader("res/shaders/shader.glsl", &vs_src, &fs_src);
	shader_program = create_shader_program(vs_src, fs_src);
	glUseProgram(shader_program);
	free(vs_src);
	free(fs_src);

	va = create_va(1);
	vb = create_vb(vertices, sizeof(vertices), GL_FLOAT, sizeof(float));
	vb_layout = create_vb_layout(vb, 3);

	vb_layout_add(vb_layout, "position", 3);
	vb_layout_add(vb_layout, "vcolor", 3);
	vb_layout_add(vb_layout, "vtexcoord", 2);

	va_add(va, vb_layout);

	va_use_shader(va, shader_program);

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

	if (!add_key(&err, XK_w, NULL, NULL, move_foreward))
		die("error adding key: %s", err);
	if (!add_key(&err, XK_s, NULL, NULL, move_backward))
		die("error adding key: %s", err);
	if (!add_key(&err, XK_a, NULL, NULL, move_left))
		die("error adding key: %s", err);
	if (!add_key(&err, XK_d, NULL, NULL, move_right))
		die("error adding key: %s", err);
	if (!add_key(&err, XK_space, NULL, NULL, move_up))
		die("error adding key: %s", err);
	if (!add_key(&err, XK_Shift_L, NULL, NULL, move_down))
		die("error adding key: %s", err);
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
	GLint proj_uni, model_uni, view_uni, override_color_uni;
	float aspect;
	vector vec;

	const float rvec_translate[3] = { 0.0f, -1.0f, 0.0f };
	const float rvec_scale[3] = { 1.0f, -1.0f, 1.0f };

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

	override_color_uni = glGetUniformLocation(shader_program, "override_color");
	glUniform3f(override_color_uni, 1.0f, 1.0f, 1.0f);

	glClearColor(0.0, 0.7, 0.7, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDrawElements(GL_TRIANGLES, sizeof(elements) / sizeof(GLuint),
	//               GL_UNSIGNED_INT, 0);
	
	/* draw cube */
	glDrawArrays(GL_TRIANGLES, 0, 36);

	/* only draw reflection when you can actually see it, in other words,
	 * only if the camera is above the plane
	 */
	if (current_camera->y > -0.5f) { 
		/* enable the stencil test to create a reflection */
		glEnable(GL_STENCIL_TEST);

		/* draw floor */
		glStencilFunc(GL_ALWAYS, 1, 0xff); /* set any stencil to 1 */
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xff); /* write to stencil buffer */
		glDepthMask(GL_FALSE); /* don't write to depth buffer */
		glClear(GL_STENCIL_BUFFER_BIT); /* clear stencil buffer */

		glDrawArrays(GL_TRIANGLES, 36, 6); /* actually draw floor */

		/* draw cube reflection */
		glStencilFunc(GL_EQUAL, 1, 0xff); /* pass if equal to 1 */
		glStencilMask(0x00); /* don't write to stencil buffer */
		glDepthMask(GL_TRUE); /* write to depth buffer */

		/* back to row-major order to manipulate */
		if (!transpose(&model, model))
			die("error transposing model matrix\n");
		/* move the cube down */
		if (!create_vector(&vec, 3))
			die("error creating vector\n");
		vector_copy_data(vec, rvec_translate);
		if (!translate(&model, model, vec))
			die("error translating model matrix\n");
		vector_copy_data(vec, rvec_scale);
		if (!scale(&model, model, vec))
			die("error scaling model matrix\n");
		/* and again, back to column-major order */
		if (!transpose(&model, model))
			die("error transposing model matrix\n");

		glUniformMatrix4fv(model_uni, 1, GL_FALSE, model.val);
		glUniform3f(override_color_uni, 0.3f, 0.4f, 0.5f); /* darken */
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glUniform3f(override_color_uni, 1.0f, 1.0f, 1.0f);
		glDisable(GL_STENCIL_TEST);
	} else {
		glDrawArrays(GL_TRIANGLES, 36, 6);
	}

	glXSwapBuffers(display, window);
}

void
cleanup(void)
{
	glDeleteProgram(shader_program);
	destroy_vb(vb);
	destroy_vb_layout(vb_layout);
	destroy_va(va);
}

int
main(int argc, char *argv[])
{
	struct GameState game_state = { setup, update, render, cleanup };

	engine_set_current_state(game_state);
	engine_run();

	return EXIT_SUCCESS;
}
