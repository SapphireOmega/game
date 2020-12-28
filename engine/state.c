#include "state.h"

struct game_state current_state;

void engine_set_current_state(struct game_state state)
{
	current_state = state;
}
