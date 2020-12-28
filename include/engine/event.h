#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

struct key {
	KeySym keysym;
	bool pressed;
	void (*on_press)(void);
	void (*on_release)(void);
	void (*while_pressed)(void);
};

struct key_handler {
	unsigned int n;
	size_t size;
	struct key *keys;
};

struct mouse {
	int x, y;
};

struct mouse_move {
	int x, y;
};

struct mouse_handler {
	void (*move)(struct mouse_move move);
};

extern struct key_handler key_handler;
extern struct mouse mouse;
extern struct mouse_handler mouse_handler;

bool init_keys(char **err);
bool add_key(char **err, KeySym keysym, void (*on_press)(void), void (*on_release)(void), void (*while_pressed)(void));
void destroy_keys(void);
void handle_events(void);

#endif /* EVENT_H */
