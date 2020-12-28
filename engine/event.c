#include "event.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "engine.h"
#include "window.h"

static void expose(XEvent *e);
static void client_message(XEvent *e);
static void key_press(XEvent *e);
static void key_release(XEvent *e);
static void mouse_motion(XEvent *e);

static void (*handler[LASTEvent])(XEvent *e) = {
	[Expose] = expose,
	[ClientMessage] = client_message,
	[KeyPress] = key_press,
	[KeyRelease] = key_release,
	[MotionNotify] = mouse_motion,
};

struct key_handler key_handler;
struct mouse mouse;
struct mouse_handler mouse_handler;

static void expose(XEvent *e)
{
	XGetWindowAttributes(display, window, &window_attribs);
	glViewport(0, 0, window_attribs.width, window_attribs.height);
}

static void client_message(XEvent *e)
{
	exit_game(EXIT_SUCCESS);
}

static void key_event(XEvent *e, bool pressed)
{
	KeySym keysym;
	XKeyEvent *ev;
	int i;

	ev = &e->xkey;
	keysym = XkbKeycodeToKeysym(display, (KeyCode)ev->keycode, 0, 0);
	for (i = 0; i < key_handler.n; i++) {
		if (key_handler.keys[i].keysym == keysym) {
			key_handler.keys[i].pressed = pressed;
			if (pressed && key_handler.keys[i].on_press)
				key_handler.keys[i].on_press();
			else if (!pressed && key_handler.keys[i].on_release)
				key_handler.keys[i].on_release();
		}
	}
}

static void key_press(XEvent *e)
{
	key_event(e, true);
}

static void key_release(XEvent *e)
{
	key_event(e, false);
}

static Bool predicate(Display *display, XEvent *event, char *arg)
{
	if (event->type == MotionNotify) {
		XMotionEvent *e = (XMotionEvent *)event;
		if (e->x == window_attribs.width / 2 && e->y == window_attribs.height / 2)
			return True;
	}

	return False;
}

static void mouse_motion(XEvent *e)
{
	struct mouse_move move;
	int x = ((XMotionEvent *)e)->x, y = ((XMotionEvent *)e)->y;
	int d; // dummy
	Window rw; // dummy
	XEvent ev;

	move.x = x - mouse.x;
	move.y = y - mouse.y;

	mouse.x = x;
	mouse.y = y;

	mouse_handler.move(move);

	XWarpPointer(display, 0, window, 0, 0, 0, 0, window_attribs.width / 2, window_attribs.height / 2);
	XCheckIfEvent(display, &ev, predicate, 0);
	mouse.x = window_attribs.width / 2;
	mouse.y = window_attribs.height / 2;
}

bool init_keys(char **err)
{
	key_handler.n = 0;
	key_handler.size = 20 * sizeof(struct key);
	key_handler.keys = (struct key *)malloc(key_handler.size);
	if (!key_handler.keys) {
		*err = strerror(errno);
		return false;
	}

	return true;
}

bool add_key(char **err, KeySym keysym, void (*on_press)(void), void (*on_release)(void), void (*while_pressed)(void))
{
	key_handler.n++;
	if (key_handler.n * sizeof(struct key) > key_handler.size) {
		key_handler.size = key_handler.n * sizeof(struct key);
		key_handler.keys = (struct key *)realloc(key_handler.keys, key_handler.size);
		if (!key_handler.keys) {
			*err = strerror(errno);
			return false;
		}
	}

	key_handler.keys[key_handler.n - 1].keysym = keysym;
	key_handler.keys[key_handler.n - 1].pressed = false;
	key_handler.keys[key_handler.n - 1].on_press = on_press;
	key_handler.keys[key_handler.n - 1].on_release = on_release;
	key_handler.keys[key_handler.n - 1].while_pressed = while_pressed;

	return true;
}

void destroy_keys(void)
{
	free(key_handler.keys);
	key_handler.keys = NULL;
	key_handler.n = 0;
	key_handler.size = 0;
}

void handle_events(void)
{
	for (;;) {
		XEvent e;
		if (!XPending(display))
			break;
		XNextEvent(display, &e);
		if (handler[e.type])
			handler[e.type](&e);
	}
	for (uint i = 0; i < key_handler.n; i++)
		if (key_handler.keys[i].pressed && key_handler.keys[i].while_pressed)
			key_handler.keys[i].while_pressed();
}
