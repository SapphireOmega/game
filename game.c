#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/timeb.h>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#include "util.h"
#include "imgload.h"
#include "trans.h"
#include "proj.h"

/* macros */
#define GLEW_STATIC
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

/* typedefs */
typedef GLXContext (*glXCreateContextAttribsARBProc)
	(Display *, GLXFBConfig, GLXContext, Bool, const int *);

/* enums */
enum shader_type { VERTEX, FRAGMENT, NONE };

/* function declarations */
static void check_glx_version(void);
static void match_fb_configs(GLXFBConfig **fbc, int *fbcnt);
static GLXFBConfig get_best_fb_config(GLXFBConfig *fbc, int fbcnt);
static GLXFBConfig get_fb_config(void);
static void create_window(XVisualInfo *vi);
static int context_error_handler(Display *display, XErrorEvent *e);
static bool extension_supported(const char *ext_list, const char *extension);
static void create_context(GLXFBConfig fbc);
static void parse_shader(const char *file, char **vs_dst, char **fs_dst);
static GLuint compile_shader(GLenum type, const char *src);
static GLuint create_shader_program(const char *vs_src, const char *fs_src);
static void setup(void);
static void handle_events(void);
static void render(void);
static void cleanup(void);
static void exit_game(void);

/* variables */
static struct timeb start_time, curr_time;
static Display *display;
static int screen_id;
static Window root_window;
static Colormap colormap;
static XWindowAttributes window_attribs;
static Window window;
static Atom delete_window;
static bool context_error = false;
static GLXContext render_context;
static GLuint shader_program;
static GLuint vao;
static GLuint vbo;
static GLuint ebo;
static struct tga_file test_image;
static GLuint tex;

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

void
check_glx_version(void)
{
	int glx_major;
	int glx_minor;

	if (!glXQueryVersion(display, &glx_major, &glx_minor) ||
	    (glx_major == 1 && glx_minor < 3) || glx_major < 1)
		die("invalid GLX version (%d, %d)\n", glx_major, glx_minor);
}

void
match_fb_configs(GLXFBConfig **fbc, int *fbcnt)
{
	static int visual_attribs[] = {
		GLX_X_RENDERABLE, True,
		GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE, GLX_RGBA_BIT,
		GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE, 8,
		GLX_GREEN_SIZE, 8,
		GLX_BLUE_SIZE, 8,
		GLX_ALPHA_SIZE, 8,
		GLX_DEPTH_SIZE, 24,
		GLX_STENCIL_SIZE, 8,
		GLX_DOUBLEBUFFER, True,
		None
	};

	printf("getting matching framebuffer configs\n");
	*fbc = glXChooseFBConfig(display, screen_id, visual_attribs, fbcnt);
	if (!fbc)
		die("failed to retrieve framebuffer config\n");
	printf("found %d matching FB configs\n", *fbcnt);
}

GLXFBConfig
get_best_fb_config(GLXFBConfig *fbc, int fbcnt)
{
	int i;
	int best_fbc = -1;
	int worst_fbc = -1;
	int best_num_samp = -1;
	int worst_num_samp = 999;
	XVisualInfo *vi;
	int samp_buf;
	int samples;

	printf("getting XVisualInfos\n");
	for (i = 0; i < fbcnt; ++i) {
		vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if (!vi)
			continue;

		glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
		glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

		printf("matching fbconfig %d,"
		       " visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
		       " SAMPLE = %d\n", i, vi->visualid, samp_buf, samples);

		if (best_fbc < 0 || samp_buf && samples > best_num_samp) {
			best_fbc = i;
			best_num_samp = samples;
		}

		if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
			worst_fbc = i;
			worst_num_samp = samples;
		}

		XFree(vi);
	}

	return fbc[best_fbc];
}

GLXFBConfig
get_fb_config(void)
{
	int fbcnt;
	GLXFBConfig *fbc;
	GLXFBConfig best_fbc;

	check_glx_version(); /* version 1.3 required for fbconfigs */
	match_fb_configs(&fbc, &fbcnt);
	best_fbc = get_best_fb_config(fbc, fbcnt);
	XFree(fbc);

	return best_fbc;
}

void
create_window(XVisualInfo *vi)
{
	XSetWindowAttributes swa;

	colormap = XCreateColormap(display, root_window, vi->visual, AllocNone);

	swa.colormap = colormap;
	swa.event_mask = ExposureMask | KeyPressMask;
	swa.border_pixel = BlackPixel(display, screen_id);
	swa.background_pixel = WhitePixel(display, screen_id);

	window = XCreateWindow(display, root_window, 0, 0, 800, 600, 0,
	                       vi->depth, InputOutput, vi->visual,
			       CWColormap | CWEventMask, &swa);
	if (!window)
		die("failed to create window\n");
	delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(display, window, &delete_window, 1);
	XStoreName(display, window, "OpenGl & Xlib test");
	printf("mapping window\n");
	XMapWindow(display, window);
}

int
context_error_handler(Display *display, XErrorEvent *e)
{
	context_error = true;
	return 0;
}

bool
extension_supported(const char *ext_list, const char *extension)
{
	const char *start;
	const char *where;
	const char *terminator;

	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	for (start = ext_list;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if ((where == start || *(where - 1) == ' ') &&
		    (*terminator == ' ' || *terminator == '\0'))
			return true;

		start = terminator;
	}

	return false;
}

void
create_context(GLXFBConfig fbc)
{
	const char *glxexts;
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	int ctxattr[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		None,
	};

	glxexts = glXQueryExtensionsString(display, screen_id);
	glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
		glXGetProcAddressARB((const GLubyte*) "glXCreateContextAttribsARB");

	render_context = 0;
	int (*old_error_handler)(Display *, XErrorEvent *) =
		XSetErrorHandler(&context_error_handler);

	if (!extension_supported(glxexts, "GLX_ARB_create_context") ||
	    !glXCreateContextAttribsARB)
	{
		printf("glXCreateContextAttribsARB() not found ... "
		       "using old-style GLX context\n");
		render_context =
			glXCreateNewContext(display, fbc, GLX_RGBA_TYPE, 0, True);
	} else {
		printf("creating context\n");
		render_context =
			glXCreateContextAttribsARB(display, fbc, 0, True, ctxattr);
		XFlush(display);
		if (context_error || !render_context) {
			ctxattr[1] = 1;
			ctxattr[3] = 0;
			context_error = false;
			printf("failed to create GL 3.0 context ... "
			       "using old-style GLX context\n");
			render_context =
				glXCreateNewContext(display, fbc, GLX_RGBA_TYPE, 0, True);
		}
	}

	XFlush(display);
	XSetErrorHandler(old_error_handler);

	if (context_error || !render_context)
		die("failed to create OpenGL context\n");

	if (glXIsDirect(display, render_context))
		printf("direct GLX rendering context obtained\n");
	else
		printf("indirect GLX rendering context obtained\n");

	printf("making context current\n");
	glXMakeCurrent(display, window, render_context);
}

void
parse_shader(const char *file, char **vs_dst, char **fs_dst)
{
	FILE *fd;
	long len;
	char *line, *arg;
	size_t n = 0;
	enum shader_type t = NONE;
	int linenum;

	if (!(fd = fopen(file, "r"))) {
		fprintf(stderr, "error opening file\n");
		exit(EXIT_FAILURE);
	}

	fseek(fd, 0, SEEK_END);
	len = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	*vs_dst = (char *)malloc(sizeof(char) * len);
	*fs_dst = (char *)malloc(sizeof(char) * len);
	if (!vs_dst) {
		fprintf(stderr, "error allocating vertex shader buffer\n");
		exit(EXIT_FAILURE);
	}

	if (!fs_dst) {
		fprintf(stderr, "error allocating fragment shader buffer\n");
		exit(EXIT_FAILURE);
	}

	*vs_dst[0] = '\0';
	*fs_dst[0] = '\0';

	for (linenum = 0;; linenum++) {
		getline(&line, &n, fd); /* automatically allocates line */
		if (ferror(fd)) {
			fprintf(stderr, "error reading from %s: %s\n",
			        file, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (feof(fd))
			break;

		if (strncmp(line, "#shader", 7) == 0) {
			arg = line + 8;
			while (*arg == ' ' || *arg == '\t')
				arg++;
			if (strncmp(arg, "vertex", 6) == 0) {
				t = VERTEX;
			} else if (strncmp(arg, "fragment", 8) == 0) {
				t = FRAGMENT;
			} else {
				fprintf(stderr, "error in %s\n", file);
				fprintf(stderr, "(line %d): %s", linenum, line);
				exit(EXIT_FAILURE);
			}
		} else {
			if (t == VERTEX)
				strcat(*vs_dst, line);
			else if (t == FRAGMENT)
				strcat(*fs_dst, line);
		}
	}

	fclose(fd);
}

GLuint
compile_shader(GLenum type, const char *src)
{
	GLuint shader;
	GLuint result;
	int len;
	char *errmsg;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		errmsg = (char *)alloca(len * sizeof(char));
		glGetShaderInfoLog(shader, len, NULL, errmsg);
		die("error compiling shader: %s\n", errmsg);
	}

	return shader;
}

GLuint
create_shader_program(const char *vs_src, const char *fs_src)
{
	GLuint vs;
	GLuint fs;
	GLuint program;

	printf("compiling vertex shader\n");
	vs = compile_shader(GL_VERTEX_SHADER, vs_src);
	printf("compiling fragment shader\n");
	fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glBindFragDataLocation(program, 0, "out_color");
	glLinkProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

void
setup(void)
{
	GLXFBConfig fbc;
	XVisualInfo *vi;
	char *vs_src, *fs_src;
	GLint pos_attrib, col_attrib, tex_attrib, status;

	ftime(&start_time);

	if (!(display = XOpenDisplay(NULL)))
		die("cannot open display\n");

	screen_id = DefaultScreen(display);
	root_window = RootWindow(display, screen_id);

	fbc = get_fb_config();
	vi = glXGetVisualFromFBConfig(display, fbc);
	printf("chosen visual id = 0x%x\n", vi->visualid);
	create_window(vi);
	XFree(vi);
	create_context(fbc);

	printf("initializing glew\n");
	glewExperimental = GL_TRUE;
	glewInit();

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
}

void
handle_events(void)
{
	XEvent e;

	for (;;) {
		if (!XPending(display))
			break;
		XNextEvent(display, &e);
		switch (e.type) {
		case Expose:
			XGetWindowAttributes(display, window, &window_attribs);
			glViewport(0, 0, window_attribs.width,
			           window_attribs.height);
			render();
			glXSwapBuffers(display, window);
			break;
		case ClientMessage:
			exit_game();
			break;
		default:
			printf("dropping unhandled XEvent.type = %d\n", e.type);
			break;
		}
	}
}

void
render(void)
{
	matrix proj, model, cam, view;
	matrix rot1, cam_rot;
	vector axis1, cam_axis;
	GLint proj_uni, model_uni, view_uni;
	int diff;
	unsigned int width, height;
	float aspect;
	unsigned int dummy;

	ftime(&curr_time);

	diff = (int)(1000.0 * (curr_time.time - start_time.time)
		+ (curr_time.millitm - start_time.millitm));

	float raxis1[] = { 1.0f, 1.0f, 0.0f };
	if (!create_vector(&axis1, 3))
		die("error creating vector axis1\n");
	vector_copy_data(axis1, raxis1);
	if (!normalize_vector(&axis1, axis1))
		die("error normalizing vector axis1\n");
	if (!create_simple_matrix(&rot1, 4, 4, 1.0f))
		die("error creating matrix\n");
	if (!rotate(&rot1, rot1, diff / 100.0f, axis1))
		die("error rotating matrix rot1\n");
	model = rot1;

	const float rcam[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	if (!create_matrix(&cam, 4, 4))
		die("error creating matrix cam_rot\n");
	matrix_copy_data(cam, rcam);

	const float rcam_axis[] = { 1.0f, 0.0f, 0.0f };
	if (!create_vector(&cam_axis, 3))
		die("error creating vector cam_axis");
	vector_copy_data(cam_axis, rcam_axis);
	if (!normalize_vector(&cam_axis, cam_axis))
		die("error normalizing vector\n");
	if (!rotate(&cam, cam, diff / 500.0f, cam_axis))
		die("error rotating matrix cam_rot\n");

	if (!inverse_matrix(&view, cam))
		die("error getting view from cam\n");

	/* TODO: check errors */
	XGetGeometry(display, window, (Window *)&dummy, (int *)&dummy,
	             (int *)&dummy, &width, &height, &dummy, &dummy);
	aspect = (float)width / (float)height;
	if (!perspective(&proj, 1.57f, aspect, 0.1f, 100.0f))
		die("error getting perspective");

	/* OpenGl uses column-major order (I found out the hard way) */
	if (!transpose(&model, model))
		die("error transposing model matrix\n");
	if (!transpose(&view, view))
		die("error transposing view matrix\n");
	if (!transpose(&proj, proj))
		die("error transposing projection matrix\n");

	model_uni = glGetUniformLocation(shader_program, "model");
	glUniformMatrix4fv(model_uni, 1, GL_FALSE, model.val);

	view_uni = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(view_uni, 1, GL_FALSE, view.val);

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

	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, render_context);
	XFreeColormap(display, colormap);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}

void
exit_game(void)
{
	cleanup();
	exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
	setup();

	for (;;) {
		handle_events();
		render();
	}

	cleanup();

	return EXIT_SUCCESS;
}
