#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "util.h"

struct BufferElement {
	const char *name;
	unsigned int count;
	unsigned int offset;
};

void
parse_shader(const char *file, char **vs_dst, char **fs_dst)
{
	FILE *fd;
	long len;
	char *line, *arg;
	size_t n = 0;
	enum shader_type t = NONE;
	int linenum;

	if (!(fd = fopen(file, "r"))) {
		fprintf(stderr, "error opening file\n");
		exit(EXIT_FAILURE);
	}

	fseek(fd, 0, SEEK_END);
	len = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	*vs_dst = (char *)malloc(sizeof(char) * len);
	*fs_dst = (char *)malloc(sizeof(char) * len);
	if (!vs_dst) {
		fprintf(stderr, "error allocating vertex shader buffer\n");
		exit(EXIT_FAILURE);
	}

	if (!fs_dst) {
		fprintf(stderr, "error allocating fragment shader buffer\n");
		exit(EXIT_FAILURE);
	}

	*vs_dst[0] = '\0';
	*fs_dst[0] = '\0';

	for (linenum = 0;; linenum++) {
		getline(&line, &n, fd); /* automatically allocates line */
		if (ferror(fd)) {
			fprintf(stderr, "error reading from %s: %s\n",
			        file, strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (feof(fd))
			break;

		if (strncmp(line, "#shader", 7) == 0) {
			arg = line + 8;
			while (*arg == ' ' || *arg == '\t')
				arg++;
			if (strncmp(arg, "vertex", 6) == 0) {
				t = VERTEX;
			} else if (strncmp(arg, "fragment", 8) == 0) {
				t = FRAGMENT;
			} else {
				fprintf(stderr, "error in %s\n", file);
				fprintf(stderr, "(line %d): %s", linenum, line);
				exit(EXIT_FAILURE);
			}
		} else {
			if (t == VERTEX)
				strcat(*vs_dst, line);
			else if (t == FRAGMENT)
				strcat(*fs_dst, line);
		}
	}

	fclose(fd);
}

GLuint
compile_shader(GLenum type, const char *src)
{
	GLuint shader;
	GLuint result;
	int len;
	char *errmsg;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
		errmsg = (char *)alloca(len * sizeof(char));
		glGetShaderInfoLog(shader, len, NULL, errmsg);
		die("error compiling shader: %s\n", errmsg);
	}

	return shader;
}

GLuint
create_shader_program(const char *vs_src, const char *fs_src)
{
	GLuint vs;
	GLuint fs;
	GLuint program;

	vs = compile_shader(GL_VERTEX_SHADER, vs_src);
	fs = compile_shader(GL_FRAGMENT_SHADER, fs_src);
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glBindFragDataLocation(program, 0, "out_color");
	glLinkProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

VertexBuffer *
create_vb(const void *data, size_t size, unsigned int type,
          size_t type_size)
{
	VertexBuffer *vb;

	vb = (VertexBuffer *)malloc(sizeof(VertexBuffer));
	if (vb == NULL)
		die("error allocating space for vertex buffer");
	vb->data = data;
	vb->size = size;
	vb->type = type;
	vb->type_size = type_size;

	glGenBuffers(1, &vb->id);
	glBindBuffer(GL_ARRAY_BUFFER, vb->id);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

	return vb;
}

void
destroy_vb(VertexBuffer *vb)
{
	glDeleteBuffers(1, &vb->id);
	free(vb);
}

BufferLayout *
create_buffer_layout(const VertexBuffer *vb, unsigned int max_count)
{
	BufferLayout *layout;

	layout = (BufferLayout *)malloc(sizeof(BufferLayout));
	if (layout == NULL)
		die("error allocating space for vertex buffer");
	layout->elems = (struct BufferElement *) \
	                malloc(sizeof(struct BufferElement) * max_count);
	layout->count = 0;
	layout->max_count = max_count;
	layout->stride = 0;
	layout->vb = vb;

	return layout;
}

void
destroy_buffer_layout(BufferLayout *layout)
{
	free(layout->elems);
	free(layout);
}

void
buffer_layout_add(BufferLayout *layout, const char *name, unsigned int count)
{
	unsigned int i;

	if (layout->count >= layout->max_count)
		die("buffer layout exceeded maximum number of elements: %s",
		    strerror(errno));

	layout->elems[layout->count].name = name;
	layout->elems[layout->count].count = count;
	layout->elems[layout->count].offset = layout->stride;

	layout->count++;
	layout->stride += count;
}

VertexArray *
create_va(unsigned int init_count)
{
	VertexArray *va;

	va = (VertexArray *)malloc(sizeof(VertexArray));
	if (va == NULL)
		die("error allocating space for vertex array: %s",
		    strerror(errno));

	va->layouts = (const BufferLayout **) \
		      malloc(init_count * sizeof(BufferLayout *));
	if (va->layouts == NULL)
		die("error allocating space for buffer layouts: %s",
		    strerror(errno));

	va->count = 0;
	va->max_count = init_count;

	glGenVertexArrays(1, &va->id);

	return va;
}

void
destroy_va(VertexArray *va)
{
	free(va->layouts);
	free(va);
}

void
va_add(VertexArray *va, const BufferLayout *layout)
{
	if (va->count == va->max_count) {
		va->max_count *= 2;
		va->layouts = (const BufferLayout **) \
			     realloc(va->layouts,
		                     va->max_count * sizeof(BufferLayout *));
		if (va->layouts == NULL)
			die("error reallocating space for buffer layouts: %s",
			    strerror(errno));
	}

	va->layouts[va->count] = layout;

	va->count++;
}

void
va_use_shader(VertexArray *va, unsigned int shader)
{
	unsigned int i, j;

	GLenum e;

	glBindVertexArray(va->id);

	for (i = 0; i < va->count; i++) {
		const BufferLayout *layout;

		layout = va->layouts[i];

		if (layout->count != layout->max_count)
			printf("warning: layout not using max element count\n");

		glBindBuffer(GL_ARRAY_BUFFER, layout->vb->id);

		for (j = 0; j < layout->count; j++) {
			const struct BufferElement *elem;
			int attrib;
			GLsizei stride;
			size_t offset;

			elem = &layout->elems[j];

			stride = layout->stride * layout->vb->type_size;
			offset = elem->offset * layout->vb->type_size;

			attrib = glGetAttribLocation(shader, elem->name);
			glEnableVertexAttribArray(attrib);
			glVertexAttribPointer(attrib, elem->count,
			                      layout->vb->type, GL_FALSE,
					      stride, (void *)offset);
		}
	}
}

void
va_bind(VertexArray *va)
{
	glBindVertexArray(va->id);
}

void
va_unbind(void)
{
	glBindVertexArray(0);
}
