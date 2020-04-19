#include "engine_time.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/* globals */
static struct timespec start, end;

/* functions */
void
init_time(void)
{
	clock_gettime(CLOCK_MONOTONIC, &end);
}

void
update_delta_time(void)
{
	start = end;
	clock_gettime(CLOCK_MONOTONIC, &end);
	delta_time = (double)(end.tv_sec - start.tv_sec) +
	             (double)(end.tv_nsec - start.tv_nsec) / 1.0e9;
	printf("dt: %f\n", delta_time);
}
