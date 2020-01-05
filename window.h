#ifndef WINDOW_H
#define WINDOW_H

#include <stdbool.h>

#include <GL/glew.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

/* macros */
#define GLEW_STATIC

/* types */
typedef GLXContext (*glXCreateContextAttribsARBProc)
	(Display *, GLXFBConfig, GLXContext, Bool, const int *);

/* functions */
static void check_glx_version(void);
static void match_fb_configs(GLXFBConfig **fbc, int *fbcnt);
static GLXFBConfig get_best_fb_config(GLXFBConfig *fbc, int fbcnt);
static GLXFBConfig get_fb_config(void);
static void create_window(XVisualInfo *vi, unsigned int w, unsigned int h);
static int context_error_handler(Display *display, XErrorEvent *e);
static bool extension_supported(const char *ext_list, const char *extension);
static void create_context(GLXFBConfig fbc);

void engine_create_window(unsigned int w, unsigned int h);
void engine_destroy_window(void);

/* variables */
Display *display;
XWindowAttributes window_attribs;
Window window;

static int screen_id;
static Window root_window;
static Colormap colormap;
static Atom delete_window;
static bool context_error = false;
static GLXContext render_context;

#endif /* WINDOW_H */
