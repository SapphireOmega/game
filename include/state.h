#ifndef STATE_H
#define STATE_H

/* TODO: stack machine */

/* types */
typedef struct {
	void (*setup)(void);
	void (*update)(void);
	void (*render)(void);
	void (*cleanup)(void);
} GameState;

/* globals */
GameState current_state;

/* functions */
void engine_set_current_state(GameState state);

#endif /* STATE_H */
