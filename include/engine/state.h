#ifndef STATE_H
#define STATE_H

/* TODO: stack machine */

/* structs */
struct GameState {
	void (*setup)(void);
	void (*update)(void);
	void (*render)(void);
	void (*cleanup)(void);
};

/* globals */
struct GameState current_state;

/* functions */
void engine_set_current_state(struct GameState state);

#endif /* STATE_H */
