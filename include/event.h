#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/* types */
typedef struct {
	KeySym keysym;
	bool pressed;
	void (*on_press)(void);
	void (*on_release)(void);
	void (*while_pressed)(void);
} Key;

typedef struct {
	unsigned int n;
	size_t size;
	Key *keys;
} KeyHandler;

typedef struct {
	int x, y;
} Mouse;

typedef struct {
	int x, y;
} MouseMove;

typedef struct {
	void (*move)(MouseMove move);
} MouseHandler;

/* functions */
bool init_keys(char *err);
bool add_key(char *err, KeySym keysym, void (*on_press)(void),
             void (*on_release)(void), void (*while_pressed)(void));
void clean_keys(void);

static void expose(XEvent *e);
static void client_message(XEvent *e);
static void key_event(XEvent *e, bool pressed);
static void key_press(XEvent *e);
static void key_release(XEvent *e);
static void mouse_motion(XEvent *e);

void handle_events(void);

/* globals */
KeyHandler key_handler;
Mouse mouse;
MouseHandler mouse_handler;

#endif /* EVENT_H */
