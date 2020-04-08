#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

/* macros */
#define GLEW_STATIC

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

/* types */
typedef GLXContext (*glXCreateContextAttribsARBProc)
	(Display *, GLXFBConfig, GLXContext, Bool, const int *);

/* functions */
void engine_create_window(unsigned int w, unsigned int h);
void engine_destroy_window(void);

/* variables */
Display *display;
XWindowAttributes window_attribs;
Window window;

#endif /* WINDOW_H */
