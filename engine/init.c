#include "init.h"

#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "engine_time.h"
#include "event.h"
#include "util.h"

#include "../game.h"

void
engine_setup(void)
{
	char *err;

	init_time();
	if (!init_keys(err))
		die("error initializing keys: %s", err);
}

void
engine_cleanup(void)
{
	clean_keys();
	XAutoRepeatOn(display);
	engine_destroy_window();
}

void
exit_game(int status)
{
	cleanup();
	engine_cleanup();
	exit(status);
}
