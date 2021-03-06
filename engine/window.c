#include "window.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "util.h"

static int screen_id;
static Window root_window;
static Colormap colormap;
static Atom delete_window;
static bool context_error = false;
static GLXContext render_context;

Display *display;
XWindowAttributes window_attribs;
Window window;

static void check_glx_version(void)
{
	int glx_major;
	int glx_minor;

	if (!glXQueryVersion(display, &glx_major, &glx_minor) || (glx_major == 1 && glx_minor < 3) || glx_major < 1)
		die("Invalid GLX version (%d, %d)\n", glx_major, glx_minor);
}

static void match_fb_configs(GLXFBConfig **fbc, int *fbcnt)
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

	printf("Getting matching framebuffer configs\n");
	*fbc = glXChooseFBConfig(display, screen_id, visual_attribs, fbcnt);
	if (!fbc)
		die("Failed to retrieve framebuffer config\n");
	printf("Found %d matching FB configs\n", *fbcnt);
}

static GLXFBConfig get_best_fb_config(GLXFBConfig *fbc, int fbcnt)
{
	int i;
	int best_fbc = -1;
	int worst_fbc = -1;
	int best_num_samp = -1;
	int worst_num_samp = 999;
	XVisualInfo *vi;
	int samp_buf;
	int samples;

	printf("Getting XVisualInfos\n");
	for (i = 0; i < fbcnt; ++i) {
		vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if (!vi)
			continue;

		glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
		glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

		printf("Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d, SAMPLE = %d\n",
		       i, vi->visualid, samp_buf, samples);

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

static GLXFBConfig get_fb_config(void)
{
	int fbcnt;
	GLXFBConfig *fbc;
	GLXFBConfig best_fbc;

	check_glx_version(); // version 1.3 required for fbconfigs
	match_fb_configs(&fbc, &fbcnt);
	best_fbc = get_best_fb_config(fbc, fbcnt);
	XFree(fbc);

	return best_fbc;
}

static void create_window(XVisualInfo *vi, unsigned int w, unsigned int h)
{
	XSetWindowAttributes swa;
	char no_data[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	Cursor invisible_cursor;
	Pixmap bitmap_no_data;
	XColor black;

	colormap = XCreateColormap(display, root_window, vi->visual, AllocNone);

	swa.colormap = colormap;
	swa.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | PointerMotionMask;
	swa.border_pixel = BlackPixel(display, screen_id);
	swa.background_pixel = WhitePixel(display, screen_id);

	window = XCreateWindow(display, root_window, 0, 0, w, h, 0,
	                       vi->depth, InputOutput, vi->visual,
	                       CWColormap | CWEventMask, &swa);
	if (!window)
		die("Failed to create window\n");
	delete_window = XInternAtom(display, "WM_DELETE_WINDOW", 0);
	XSetWMProtocols(display, window, &delete_window, 1);
	XStoreName(display, window, "OpenGl & Xlib test");
	printf("Mapping window\n");
	XMapWindow(display, window);

	black.red = black.green = black.blue = 0;
	bitmap_no_data = XCreateBitmapFromData(display, window, no_data, 8, 8);
	invisible_cursor = XCreatePixmapCursor(display, bitmap_no_data, bitmap_no_data, &black, &black, 0, 0);
	XDefineCursor(display, window, invisible_cursor);
	XFreeCursor(display, invisible_cursor);
	XFreePixmap(display, bitmap_no_data);
}

static int context_error_handler(Display *display, XErrorEvent *e)
{
	context_error = true;
	return 0;
}

static bool extension_supported(const char *ext_list, const char *extension)
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

static void create_context(GLXFBConfig fbc)
{
	const char *glxexts;
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	int ctxattr[] = {
		GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
		GLX_CONTEXT_MINOR_VERSION_ARB, 3,
		None,
	};

	glxexts = glXQueryExtensionsString(display, screen_id);
	glXCreateContextAttribsARB =
		(glXCreateContextAttribsARBProc)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

	render_context = 0;
	int (*old_error_handler)(Display *, XErrorEvent *) = XSetErrorHandler(&context_error_handler);

	if (!extension_supported(glxexts, "GLX_ARB_create_context") || !glXCreateContextAttribsARB) {
		printf("glXCreateContextAttribsARB() not found ... using old-style GLX context\n");
		render_context = glXCreateNewContext(display, fbc, GLX_RGBA_TYPE, 0, True);
	} else {
		printf("Creating context\n");
		render_context = glXCreateContextAttribsARB(display, fbc, 0, True, ctxattr);
		XFlush(display);
		if (context_error || !render_context) {
			ctxattr[1] = 1;
			ctxattr[3] = 0;
			context_error = false;
			printf("failed to create GL 3.0 context ... using old-style GLX context\n");
			render_context = glXCreateNewContext(display, fbc, GLX_RGBA_TYPE, 0, True);
		}
	}

	XFlush(display);
	XSetErrorHandler(old_error_handler);

	if (context_error || !render_context)
		die("Failed to create OpenGL context\n");

	if (glXIsDirect(display, render_context))
		printf("Direct GLX rendering context obtained\n");
	else
		printf("Indirect GLX rendering context obtained\n");

	printf("Making context current\n");
	glXMakeCurrent(display, window, render_context);
}

void engine_create_window(unsigned int w, unsigned int h)
{

	GLXFBConfig fbc;
	XVisualInfo *vi;

	if (!(display = XOpenDisplay(NULL)))
		die("Cannot open display\n");

	screen_id = DefaultScreen(display);
	root_window = RootWindow(display, screen_id);

	fbc = get_fb_config();
	vi = glXGetVisualFromFBConfig(display, fbc);
	printf("Chosen visual id = 0x%x\n", vi->visualid);
	create_window(vi, w, h);
	XFree(vi);
	create_context(fbc);

	printf("Initializing GLEW\n");
	glewExperimental = GL_TRUE;
	GLuint error = glewInit();
	if (error != GLEW_OK)
		die("error initializing glew: %s\n", glewGetErrorString(error));
}

void engine_destroy_window(void)
{
	glXMakeCurrent(display, None, NULL);
	glXDestroyContext(display, render_context);
	XFreeColormap(display, colormap);
	XDestroyWindow(display, window);
	XCloseDisplay(display);
}
