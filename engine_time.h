#ifndef ENGINE_TIME_H
#define ENGINE_TIME_H

#include <time.h>

/* functions */
void init_time(void);
void update_delta_time(void);

double delta_time;

static struct timespec start, end;

#endif /* ENGINE_TIME_H */
