#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/* types */
struct Key {
	KeySym keysym;
	bool pressed;
	void (*on_press)(void);
	void (*on_release)(void);
	void (*while_pressed)(void);
};

struct KeyHandler {
	unsigned int n;
	size_t size;
	struct Key *keys;
};

struct Mouse {
	int x, y;
};

struct MouseMove {
	int x, y;
};

struct MouseHandler {
	void (*move)(struct MouseMove move);
};

/* globals */
extern struct KeyHandler key_handler;
extern struct Mouse mouse;
extern struct MouseHandler mouse_handler;

/* functions */
bool init_keys(char **err);
bool add_key(char **err, KeySym keysym, void (*on_press)(void),
             void (*on_release)(void), void (*while_pressed)(void));
void destroy_keys(void);
void handle_events(void);

#endif /* EVENT_H */
