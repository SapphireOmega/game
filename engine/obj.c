#include "obj.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

void load_obj_file(struct OBJ_FILE *file, const char *filename)
{
	FILE *fd;
	if (!(fd = fopen(filename, "r"))) {
		fprintf(stderr, "Error opening file %s: %s", filename, strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	file->v_size = file->f_size = 0;

	for (;;) {
		char *line;
		size_t n = 0;

		if (!getline(&line, &n, fd)) {
			fprintf(stderr, "error reading from %s: %s\n", file, strerror(errno));
			exit_game(EXIT_FAILURE);
		}

		if (feof(fd))
			break;

		switch(line[0]) {
		case '#':
		case 's':
			break;
		case 'v':
			file->v_size += 3;
			break;
		case 'f':
			file->f_size += 3;
			break;
		default:
			fprintf(stderr, "Unknown character while reading OBJ file: %c\n", line[0]);
			exit_game(EXIT_FAILURE);
		}

		free(line);
	}
	
	file->vs = (float *)malloc(file->v_size * sizeof(float));
	if (!file->vs) {
		fprintf(stderr, "Error allocating vs while loading OBJ file: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	file->fs = (uint *)malloc(file->f_size * sizeof(float));
	if (!file->fs) {
		fprintf(stderr, "Error allocating fs while loading OBJ file: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	fseek(fd, 0, SEEK_SET);

	uint v_index = 0, f_index = 0;
	for (;;) {
		char *line;
		size_t n = 0;

		if (!getline(&line, &n, fd)) {
			fprintf(stderr, "error reading from %s: %s\n", file, strerror(errno));
			exit_game(EXIT_FAILURE);
		}

		if (feof(fd))
			break;

		char *arg = line;
		char num[16];

		switch(line[0]) {
		case '#':
		case 's':
			break;
		case 'v':
			while (*arg != ' ')
				arg++;
			arg++;
			for (uint i = 0; i < 3; i++) {
				size_t num_len;
				for (num_len = 1; arg[num_len] != ' ' && arg[num_len] != '\n'; num_len++);
				strncpy(num, arg, num_len);
				num[num_len] = '\0';
				sscanf(num, "%f", &file->vs[v_index++]);
				arg += num_len;
			}
			break;
		case 'f':
			while (*arg != ' ')
				arg++;
			arg++;
			for (uint i = 0; i < 3; i++) {
				size_t num_len;
				for (num_len = 1; arg[num_len] != ' ' && arg[num_len] != '\n'; num_len++);
				strncpy(num, arg, num_len);
				num[num_len] = '\0';
				sscanf(num, "%d", &file->fs[f_index++]);
				arg += num_len;
			}
			break;
		default:
			fprintf(stderr, "Unknown character while reading OBJ file: %c\n", line[0]);
			exit_game(EXIT_FAILURE);
		}

		free(line);
	}

	fclose(fd);
}
