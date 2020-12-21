#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <GL/glx.h>
#include <X11/Xlib.h>

#define GLEW_STATIC

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display *, GLXFBConfig, GLXContext, Bool, const int *);

void engine_create_window(unsigned int w, unsigned int h);
void engine_destroy_window(void);

extern Display *display;
extern XWindowAttributes window_attribs;
extern Window window;

#endif /* WINDOW_H */
