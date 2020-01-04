#ifndef GAME_H
#define GAME_H

#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include "util.h"
#include "imgload.h"
#include "trans.h"
#include "proj.h"
#include "window.h"

/* macros */
#define GLEW_STATIC
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

#define LENGTH(X) sizeof(X) / sizeof(X[0])

/* types */
typedef struct {
	KeySym keysym;
	bool pressed;
	void (*func)(void);
} Key;

/* enums */
enum shader_type { VERTEX, FRAGMENT, NONE };

/* function declarations */
static void parse_shader(const char *file, char **vs_dst, char **fs_dst);
static GLuint compile_shader(GLenum type, const char *src);
static GLuint create_shader_program(const char *vs_src, const char *fs_src);

void expose(XEvent *e);
void client_message(XEvent *e);
void key_event(XEvent *e, bool pressed);
void key_press(XEvent *e);
void key_release(XEvent *e);

static void (*handler[LASTEvent])(XEvent *e) = {
	[Expose] = expose,
	[ClientMessage] = client_message,
	[KeyPress] = key_press,
	[KeyRelease] = key_release,
};

void move_foreward(void);
void move_backward(void);
void move_left(void);
void move_right(void);
void rot_left(void);
void rot_right(void);

void setup(void);
void handle_events(void);
void render(void);
void cleanup(void);
void exit_game(void);

/* globals */
GLuint shader_program;
GLuint vao;
GLuint vbo;
GLuint ebo;
struct tga_file test_image;
GLuint tex;
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

static Key keys[] = {
	{ XK_w, false, move_foreward },
	{ XK_s, false, move_backward },
	{ XK_a, false, move_left },
	{ XK_d, false, move_right },
	{ XK_q, false, rot_left },
	{ XK_e, false, rot_right },
};

#endif /* GAME_H */
