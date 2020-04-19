#include "engine.h"

#include <stdlib.h>
#include <stdbool.h>

#include "engine_time.h"
#include "event.h"
#include "state.h"
#include "util.h"
#include "window.h"

/* globals */
static bool running = true;

/* functions */
static void
engine_setup(void)
{
	char *err;

	init_time();
	if (!init_keys(&err))
		die("error initializing keys: %s", err);
}

static void
engine_cleanup(void)
{
	destroy_keys();
	//XAutoRepeatOn(display);
	engine_destroy_window();
}

void
engine_run(void)
{
	engine_setup();
	if (current_state.setup)
		current_state.setup();

	while (running) {
		update_delta_time();
		handle_events();
		if (current_state.update)
			current_state.update();
		if (current_state.render)
			current_state.render();
	}

	if (current_state.cleanup)
		current_state.cleanup();
}

void
exit_game(int status)
{
	if (current_state.cleanup)
		current_state.cleanup();
	engine_cleanup();
	running = false;
}
