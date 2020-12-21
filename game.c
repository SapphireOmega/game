/* TODO:
 * load obj
 * lights
 * error checking in the functions instead of by the program
 * fix the X connection broken message
 * shader abstraction
 * texture abstraction
 * mipmaps
 * opengl error logging
 * gamestate stack
 * collision
 * reflection
 * materials
 * sound
 */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <GL/glew.h>

#include <engine/engine.h>
#include <engine/engine_time.h>
#include <engine/event.h>
#include <engine/shader.h>
#include <engine/state.h>
#include <engine/imgload.h>
#include <engine/proj.h>
#include <engine/vec.h>
#include <engine/util.h>
#include <engine/window.h>

/* variables */
static GLuint shader_program;
static VertexBuffer *vb;
static VertexBufferLayout *vb_layout;
static VertexArray *va;
static GLuint ib;
static struct TGA_File test_image;
static GLuint tex;
static struct Camera cam = {
	.x = 0.0f, .y = 0.0f, .z = 3.0f,
	.angle_x = 0.0f, .angle_y = 0.0f, .angle_z = 0.0f,
	.fovx = 1.570796f,
	.proj = PERSP,
	.n = 0.05f, .f = 100.0f
};

static const float vertices[] = {
/*      pos                  color            */
	/* front */
	-0.5f,  0.5f,  0.5f, 1.0f, 0.4f, 1.0f,
	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.4f,
	-0.5f, -0.5f,  0.5f, 1.0f, 0.4f, 0.4f,

	/* back */
	-0.5f,  0.5f, -0.5f, 0.4f, 0.4f, 1.0f,
	 0.5f,  0.5f, -0.5f, 0.4f, 1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f, 0.4f, 1.0f, 0.4f,
	-0.5f, -0.5f, -0.5f, 0.4f, 0.4f, 0.4f,

	/* floor */
	-1.0f, -0.5001f, -1.0f, 1.0f, 1.0f, 1.0f,
	 1.0f, -0.5001f, -1.0f, 0.4f, 1.0f, 1.0f,
	 1.0f, -0.5001f,  1.0f, 0.4f, 0.4f, 1.0f,
	-1.0f, -0.5001f,  1.0f, 1.0f, 0.4f, 1.0f,
};

//static const float vertices[] = {
///*      pos                  color             texcoords */
//	/* front */
//	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//
//	/* back */
//	 0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	 0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//	 0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//
//	/* left */
//	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//
//	/* right */
//	 0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	 0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//	 0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//
//	 /* top */
//	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f,  0.5f,  0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//	-0.5f,  0.5f, -0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
// 	
//	/* bottom */
//	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//	 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
//	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.5f, 1.0f, 0.0f,
//	-0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, 0.0f, 0.0f,
//	-0.5f, -0.5f,  0.5f, 1.0f, 0.5f, 1.0f, 0.0f, 1.0f,
//
//	/* floor */
//	-1.5f, -0.5001f, -1.5f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//	 1.5f, -0.5001f, -1.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//	 1.5f, -0.5001f,  1.5f, 0.0f, 1.0f, 2.0f, 0.0f, 0.0f,
//	 1.5f, -0.5001f,  1.5f, 0.0f, 1.0f, 2.0f, 0.0f, 0.0f,
//	-1.5f, -0.5001f,  1.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
//	-1.5f, -0.5001f, -1.5f, 2.0f, 0.0f, 1.0f, 0.0f, 0.0f,
//};

static const GLuint indices[] = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
	4, 0, 3, 3, 7, 4,
	1, 5, 6, 6, 2, 1,
	4, 5, 1, 1, 0, 4,
	3, 2, 6, 6, 7, 3,
	8, 9, 10, 10, 11, 8
};

float vel[3], cube_rot_axis[3];
float cube_rot[4][4], cube_init_transform[4][4];

void move_foreward(void)
{
	vel[0] += -1.0f * cosf(M_PI_2 - cam.angle_y);
	vel[2] += -1.0f * cosf(cam.angle_y);
}

void move_backward(void)
{
	vel[0] += 1.0f * cosf(M_PI_2 - cam.angle_y);
	vel[2] += 1.0f * cosf(cam.angle_y);
}

void move_left(void)
{
	vel[0] += -1.0f * cosf(cam.angle_y);
	vel[2] += 1.0f * cosf(M_PI_2 - cam.angle_y);
}

void move_right(void)
{
	vel[0] += 1.0f * cosf(cam.angle_y);
	vel[2] += -1.0f * cosf(M_PI_2 - cam.angle_y);
}

void move_up(void)
{
	vel[1] += 1.0f;
}

void move_down(void)
{
	vel[1] -= 1.0f;
}

void rot(float a, float b)
{
	cam.angle_y += a * (float)delta_time;
	cam.angle_x += b * (float)delta_time;
	if (cam.angle_x > M_PI / 2.0f)
		cam.angle_x = M_PI / 2.0f;
	else if (cam.angle_x < -M_PI / 2.0f)
		cam.angle_x = -M_PI / 2.0f;
}

void mouse_move(struct MouseMove m)
{
	rot((float)m.x * -0.02f, (float)m.y * -0.02f);
}

void quit(void)
{
	exit_game(EXIT_SUCCESS);
}

void setup(void)
{
	char *vs_src, *fs_src, *err;

	engine_create_window(800, 600);

	parse_shader("res/shaders/shader.glsl", &vs_src, &fs_src);
	shader_program = create_shader_program(vs_src, fs_src);
	free(vs_src);
	free(fs_src);

	va = create_va(1);
	vb = create_vb(vertices, sizeof(vertices), GL_FLOAT, sizeof(float));

	vb_layout = create_vb_layout(vb, 2);
	vb_layout_add(vb_layout, "position", 3);
	vb_layout_add(vb_layout, "vcolor", 3);

	va_add(va, vb_layout);

	va_use_shader(va, shader_program);

	glGenBuffers(1, &ib);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	if (!load_tga_file(&test_image, "res/textures/test.tga"))
		die("error loading tga file: %s\n", img_strerror(img_err));

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, test_image.width, test_image.height, 0, GL_RGBA, GL_FLOAT, test_image.data); 
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
	if (!add_key(&err, XK_Escape, NULL, NULL, quit))
		die("error adding key: %s", err);
	mouse_handler.move = mouse_move;

	vel[0] = vel[1] = vel[2] = 0.0f;

	current_camera = &cam;

	cube_rot_axis[0] = 0.0f;
	cube_rot_axis[1] = 1.0f;
	cube_rot_axis[2] = 0.0f;
	normalize(3, cube_rot_axis, cube_rot_axis);
	identity(4, 4, cube_rot);

	identity(4, 4, cube_init_transform);

	float cube_init_rot_axis[3] = { 1.0f, 0.0f, 1.0f };
	normalize(3, cube_init_rot_axis, cube_init_rot_axis);
	rotation_3d_homogeneous(cube_init_transform, cube_init_rot_axis, M_PI_2 - atanf(1.0f / sqrtf(2.0f)));
	float cube_init_transl[3] = { 0.0f, (sqrtf(3.0f) - 1.0f) * 0.5f, 0.0f };
	add_translation(3, cube_init_transform, cube_init_transform, cube_init_transl);
}

void update(void)
{
	normalize(3, vel, vel);

	cam.x += vel[0] * (float)delta_time;
	cam.y += vel[1] * (float)delta_time;
	cam.z += vel[2] * (float)delta_time;

	vel[0] = vel[1] = vel[2] = 0.0f;

	float d_cube_rot[4][4];
	float tmp[4][4];

	rotation_3d_homogeneous(d_cube_rot, cube_rot_axis, 1.0f * delta_time);
	matrix_matrix_product(4, 4, 4, tmp, d_cube_rot, cube_rot);
	memcpy(cube_rot, tmp, 16 * sizeof(float));
}

void render(void)
{
	float proj[4][4], model[4][4], view[4][4], projt[4][4], modelt[4][4], viewt[4][4], i[4][4];
	GLint proj_uni, model_uni, view_uni, override_color_uni;
	float aspect;

	identity(4, 4, i);

	fps_view(view);
	aspect = (float)window_attribs.width / (float)window_attribs.height;
	projection(proj, aspect);
	matrix_matrix_product(4, 4, 4, model, cube_rot, cube_init_transform);

	/* OpenGl uses column-major order (I found out the hard way) */
	transpose(4, 4, modelt, model);
	transpose(4, 4, viewt, view);
	transpose(4, 4, projt, proj);

	model_uni = glGetUniformLocation(shader_program, "model");
	glUniformMatrix4fv(model_uni, 1, GL_FALSE, (float *)modelt);

	view_uni = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(view_uni, 1, GL_FALSE, (float *)viewt);

	proj_uni = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(proj_uni, 1, GL_FALSE, (float *)projt);

	override_color_uni = glGetUniformLocation(shader_program, "override_color");
	glUniform3f(override_color_uni, 1.0f, 1.0f, 1.0f);

	glClearColor(0.0, 0.7, 0.7, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* draw cube */
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	/* only draw reflection when you can actually see it, in other words,
	 * only if the camera is above the plane
	 */
	if (current_camera->y > -0.5f) { 
		/* enable the stencil test to create a reflection */
		glEnable(GL_STENCIL_TEST);

		/* draw floor */
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, (float *)i);
		glStencilFunc(GL_ALWAYS, 1, 0xff); // set any stencil to 1
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xff); // write to stencil buffer
		glDepthMask(GL_FALSE); // don't write to depth buffer
		glClear(GL_STENCIL_BUFFER_BIT); // clear stencil buffer
	
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid *)(36 * sizeof(uint)));
		//glDrawArrays(GL_TRIANGLES, 36, 6);

		/* draw cube reflection */
		glStencilFunc(GL_EQUAL, 1, 0xff); // pass if equal to 1
		glStencilMask(0x00); // don't write to stencil buffer
		glDepthMask(GL_TRUE); // write to depth buffer

		float translation_vec[3] = { 0.0f, -0.5 * (sqrtf(3.0f) + 1.0f), 0.0f };
		float scale_vec[3] = { 1.0f, -1.0f, 1.0f };

		float reflection_scale[4][4];
		float reflection_model[4][4];
		float reflection_model_t[4][4];

		scale_homogeneous(3, reflection_scale, scale_vec);
		matrix_matrix_product(4, 4, 4, reflection_model, reflection_scale, model);
		add_translation(3, reflection_model, reflection_model, translation_vec);
		transpose(4, 4, reflection_model_t, reflection_model);

		glUniformMatrix4fv(model_uni, 1, GL_FALSE, (float *)reflection_model_t);
		glUniform3f(override_color_uni, 0.5f, 0.6f, 1.0f); /* darken */
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glUniform3f(override_color_uni, 1.0f, 1.0f, 1.0f);

		/* disable stencil test again */
		glDisable(GL_STENCIL_TEST);
	} else {
		glUniformMatrix4fv(model_uni, 1, GL_FALSE, (float *)i);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid *)(36 * sizeof(uint)));
		//glDrawArrays(GL_TRIANGLES, 36, 6);
	}

	glXSwapBuffers(display, window);
}

void cleanup(void)
{
	glDeleteProgram(shader_program);
	destroy_vb(vb);
	destroy_vb_layout(vb_layout);
	destroy_va(va);
}

int main(int argc, char *argv[])
{
	struct GameState game_state = { setup, update, render, cleanup };

	engine_set_current_state(game_state);
	engine_run();

	return EXIT_SUCCESS;
}
