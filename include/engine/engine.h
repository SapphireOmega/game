#ifndef ENGINE_H
#define ENGINE_H

typedef unsigned int uint;

#define LEN(...) sizeof(__VA_ARGS__) / sizeof(__VA_ARGS__[0])

void engine_run(void);
void exit_game(int status);

#endif /* ENGINE_H */
