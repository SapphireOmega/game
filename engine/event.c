#include "event.h"

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "window.h"
#include "init.h"

/* globals */
static void (*handler[LASTEvent])(XEvent *e) = {
	[Expose] = expose,
	[ClientMessage] = client_message,
	[KeyPress] = key_press,
	[KeyRelease] = key_release,
};

/* functions */
bool
init_keys(char *err)
{
	key_handler.n = 0;
	key_handler.size = 20 * sizeof(Key);
	key_handler.keys = (Key *)malloc(key_handler.size);
	if (!key_handler.keys) {
		err = strerror(errno);
		return false;
	}

	return true;
}

bool
add_key(char *err, KeySym keysym, void (*on_press)(void),
        void (*on_release)(void), void (*while_pressed)(void))
{
	key_handler.n++;
	if (key_handler.n * sizeof(Key) > key_handler.size) {
		key_handler.size = key_handler.n * sizeof(Key);
		key_handler.keys = \
			(Key *)realloc(key_handler.keys, key_handler.size);
		if (!key_handler.keys) {
			err = strerror(errno);
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

void
clean_keys(void)
{
	free(key_handler.keys);
	key_handler.keys = NULL;
	key_handler.n = 0;
	key_handler.size = 0;
}

void
expose(XEvent *e)
{
	XGetWindowAttributes(display, window, &window_attribs);
	glViewport(0, 0, window_attribs.width, window_attribs.height);
}

void
client_message(XEvent *e)
{
	exit_game(EXIT_SUCCESS);
}

void
key_event(XEvent *e, bool pressed)
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

void
key_press(XEvent *e)
{
	key_event(e, true);
}

void
key_release(XEvent *e)
{
	key_event(e, false);
}

void
handle_events(void)
{
	XEvent e;
	int i;

	for (;;) {
		if (!XPending(display))
			break;
		XNextEvent(display, &e);
		if (handler[e.type])
			handler[e.type](&e);
	}
	for (i = 0; i < key_handler.n; i++)
		if (key_handler.keys[i].pressed && \
		    key_handler.keys[i].while_pressed)
			key_handler.keys[i].while_pressed();
}
