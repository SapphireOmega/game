#include "game.h"

#include "engine_time.h"

int
main(int argc, char *argv[])
{
	init_time();
	setup();

	for (;;) {
		update_delta_time();

		handle_events();
		render();
	}

	cleanup();

	return EXIT_SUCCESS;
}
