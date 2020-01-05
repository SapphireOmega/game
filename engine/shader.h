#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glu.h>

/* enums */
enum shader_type { VERTEX, FRAGMENT, NONE };

/* functions */
static GLuint compile_shader(GLenum type, const char *src);

void parse_shader(const char *file, char **vs_dst, char **fs_dst);
GLuint create_shader_program(const char *vs_src, const char *fs_src);

#endif /* SHADER_H */
