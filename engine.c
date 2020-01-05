#include "game.h"

#include "engine_time.h"
#include "init.h"
#include "event.h"

int
main(int argc, char *argv[])
{
	engine_setup();
	setup();

	for (;;) {
		update_delta_time();

		handle_events();
		render();
	}

	cleanup();
	engine_cleanup();

	return EXIT_SUCCESS;
}
