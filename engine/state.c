#include "state.h"

/* variables */
struct GameState current_state;

void
engine_set_current_state(struct GameState state)
{
	current_state = state;
}
