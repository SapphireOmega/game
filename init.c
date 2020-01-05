#include "init.h"

#include <stdio.h>
#include <stdlib.h>

#include "window.h"
#include "engine_time.h"

#include "game.h"

void
engine_setup(void)
{
	init_time();
}

void
engine_cleanup(void)
{
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
