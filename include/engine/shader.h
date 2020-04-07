#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/glx.h>
#include <GL/glu.h>

/* enums */
enum shader_type { VERTEX, FRAGMENT, NONE };

/* types */
typedef struct {
	unsigned int id;
	const void *data;
	size_t size, type_size;
	unsigned int type;
} VertexBuffer;

typedef struct {
	const VertexBuffer *vb;
	struct BufferElement *elems;
	unsigned int count;
	unsigned int max_count;
	unsigned int stride;
} BufferLayout;

typedef struct {
	unsigned int id;
	const BufferLayout **layouts;
	unsigned int count, max_count;
} VertexArray;

/* functions */
static GLuint compile_shader(GLenum type, const char *src);

void parse_shader(const char *file, char **vs_dst, char **fs_dst);
GLuint create_shader_program(const char *vs_src, const char *fs_src);

VertexBuffer *create_vb(const void *data, size_t size, unsigned int type,
                        size_t type_size);
void destroy_vb(VertexBuffer *vb);

BufferLayout *create_buffer_layout(const VertexBuffer *vb,
                                   unsigned int max_count);
void destroy_buffer_layout(BufferLayout *layout);
void buffer_layout_add(BufferLayout *layout, const char *name,
                       unsigned int count);

VertexArray *create_va(unsigned int init_count);
void destroy_va(VertexArray *va);
void va_add(VertexArray *va, const BufferLayout *layout);
void va_use_shader(VertexArray *va, unsigned int shader); /* also binds va */
void va_bind(VertexArray *va);
void va_unbind(void);

#endif /* SHADER_H */
