#ifndef STATE_H
#define STATE_H

/* TODO: stack machine */

struct game_state {
	void (*setup)(void);
	void (*update)(void);
	void (*render)(void);
	void (*cleanup)(void);
};

extern struct game_state current_state;

void engine_set_current_state(struct game_state state);

#endif /* STATE_H */
