#include "obj.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

void load_obj_file(struct obj_file *file, const char *filename)
{
	FILE *fd;
	if (!(fd = fopen(filename, "r"))) {
		fprintf(stderr, "Error opening file %s: %s", filename, strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	file->vs_size = file->fs_size = file->vns_size = 0;

	for (;;) {
		char *line;
		size_t n = 0;

		if (!getline(&line, &n, fd)) {
			fprintf(stderr, "Error reading from %s: %s\n", file, strerror(errno));
			exit_game(EXIT_FAILURE);
		}

		if (feof(fd))
			break;

		switch(line[0]) {
		case '#':
		case 's':
			break;
		case 'v':
			switch(line[1]) {
			case ' ':
				file->vs_size += 3;
				break;
			case 'n':
				file->vns_size += 3;
				break;
			default:
				fprintf(stderr, "Unexpected character while reading OBJ file: %c\n", line[1]);
				exit_game(EXIT_FAILURE);
			}
			break;
		case 'f':
			file->fs_size += 3;
			break;
		default:
			fprintf(stderr, "Unexpected character while reading OBJ file: %c\n", line[0]);
			exit_game(EXIT_FAILURE);
		}

		free(line);
	}
	
	file->vs = (float *)malloc(file->vs_size * sizeof(float));
	if (!file->vs) {
		fprintf(stderr, "Error allocating vs while loading OBJ file: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	file->fs = (uint *)malloc(file->fs_size * sizeof(float));
	if (!file->fs) {
		fprintf(stderr, "Error allocating fs while loading OBJ file: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	file->vns = (float *)malloc(file->vns_size * sizeof(float));
	if (!file->vns) {
		fprintf(stderr, "Error allocating vns while loading OBJ file: %s\n", strerror(errno));
		exit_game(EXIT_FAILURE);
	}

	if (file->vns_size != 0) {
		file->vns_indices = (uint *)malloc(file->fs_size / 3 * sizeof(float));
		if (!file->vns_indices) {
			fprintf(stderr, "Error allocating vns_indices while loading OBJ file: %s\n", strerror(errno));
			exit_game(EXIT_FAILURE);
		}
	} else {
		file->vns_indices = NULL;
	}

	fseek(fd, 0, SEEK_SET);

	uint vs_index = 0, fs_index = 0, vns_index = 0, vns_indices_index = 0;
	for (;;) {
		char *line;
		size_t n = 0;

		if (!getline(&line, &n, fd)) {
			fprintf(stderr, "Error reading from %s: %s\n", file, strerror(errno));
			exit_game(EXIT_FAILURE);
		}

		if (feof(fd))
			break;

		char *arg = line;
		char num[16];
		size_t num_len;

		switch(line[0]) {
		case '#':
		case 's':
			break;
		case 'v':
			switch(line[1]) {
			case ' ':
				arg += 2;
				for (uint i = 0; i < 3; i++) {
					for (num_len = 1; arg[num_len] != ' ' && arg[num_len] != '\n'; num_len++);
					strncpy(num, arg, num_len);
					num[num_len] = '\0';
					sscanf(num, "%f", &file->vs[vs_index++]);
					arg += num_len + 1;
				}
				break;
			case 'n':
				arg += 3;
				for (uint i = 0; i < 3; i++) {
					for (num_len = 1; arg[num_len] != ' ' && arg[num_len] != '\n'; num_len++);
					strncpy(num, arg, num_len);
					num[num_len] = '\0';
					sscanf(num, "%f", &file->vns[vns_index++]);
					arg += num_len + 1;
				}
				break;
			default:
				fprintf(stderr, "Unexpected character while reading OBJ file: %c\n", line[1]);
				exit_game(EXIT_FAILURE);
			}
			break;
		case 'f':
			arg += 2;
			if (file->vns_size == 0) {
				for (uint i = 0; i < 3; i++) {
					for (num_len = 1; arg[num_len] != ' ' && arg[num_len] != '\n'; num_len++);
					strncpy(num, arg, num_len);
					num[num_len] = '\0';
					sscanf(num, "%d", &file->fs[fs_index++]);
					arg += num_len + 1;
				}
			} else {
				for (uint i = 0; i < 3; i++) {
					for (num_len = 1; arg[num_len] != '/'; num_len++);
					strncpy(num, arg, num_len);
					num[num_len] = '\0';
					sscanf(num, "%d", &file->fs[fs_index++]);
					arg += num_len + 2;

					for (num_len = 1; arg[num_len] != ' ' && arg[num_len] != '\n'; num_len++);
					if (i == 0) {
						strncpy(num, arg, num_len);
						num[num_len] = '\0';
						sscanf(num, "%d", &file->vns_indices[vns_indices_index++]);
					}
					arg += num_len + 1;
				}
			}
			break;
		default:
			fprintf(stderr, "Unexpected character while reading OBJ file: %c\n", line[0]);
			exit_game(EXIT_FAILURE);
		}

		free(line);
	}

	fclose(fd);
}

void destroy_obj_file(struct obj_file *file)
{
	free(file->vs);
	free(file->fs);
	free(file->vns);
	free(file->vns_indices);
}
