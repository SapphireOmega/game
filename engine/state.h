#ifndef STATE_H
#define STATE_H

/* TODO: stack machine */

struct GameState {
	void (*setup)(void);
	void (*update)(void);
	void (*render)(void);
	void (*cleanup)(void);
};

extern struct GameState current_state;

void engine_set_current_state(struct GameState state);

#endif /* STATE_H */
