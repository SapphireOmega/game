#ifndef ENGINE_H
#define ENGINE_H

#include <stdbool.h>

/* globals */
static bool running = true;

/* functions */
static void engine_setup(void);
static void engine_cleanup(void);

void engine_run();
void exit_game(int status);

#endif /* ENGINE_H */
