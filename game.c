#include "game.h"

#include "engine_time.h"

/* function definitions */
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

	printf("compiling vertex shader\n");
	vs = compile_shader(GL_VERTEX_SHADER, vs_src);
	printf("compiling fragment shader\n");
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

void
expose(XEvent *e)
{
	XGetWindowAttributes(display, window, &window_attribs);
	glViewport(0, 0, window_attribs.width, window_attribs.height);
}

void
client_message(XEvent *e)
{
	exit_game();
}

void
key_event(XEvent *e, bool pressed)
{
	KeySym keysym;
	XKeyEvent *ev;
	int i;

	ev = &e->xkey;
	keysym = XkbKeycodeToKeysym(display, (KeyCode)ev->keycode, 0, 0);
	for (i = 0; i < LENGTH(keys); i++)
		if (keys[i].keysym == keysym)
			keys[i].pressed = pressed;
}

void
key_press(XEvent *e)
{
	key_event(e, true);
}

void
key_release(XEvent *e)
{
	key_event(e, false);
}

void
move_foreward(void)
{
	cam.z -= 50.0f * cosf(cam.angle_y) * (float)delta_time;
	cam.x -= 50.0f * cosf(1.57f - cam.angle_y) * (float)delta_time;
}

void
move_backward(void)
{
	cam.z += 50.0f * cosf(cam.angle_y) * (float)delta_time;
	cam.x += 50.0f * cosf(1.57f - cam.angle_y) * (float)delta_time;
}

void
move_left(void)
{
	cam.z += 50.0f * cosf(1.57f - cam.angle_y) * (float)delta_time;
	cam.x -= 50.0f * cosf(cam.angle_y) * (float)delta_time;
}

void
move_right(void)
{
	cam.z -= 50.0f * cosf(1.57f - cam.angle_y) * (float)delta_time;
	cam.x += 50.0f * cosf(cam.angle_y) * (float)delta_time;
}

void
rot_left(void)
{
	cam.angle_y += 50.0f * (float)delta_time;
}

void
rot_right(void)
{
	cam.angle_y -= 50.0f * (float)delta_time;
}

void
setup(void)
{
	GLint pos_attrib, col_attrib, tex_attrib, status;
	char *vs_src, *fs_src;

	engine_create_window(800, 600);
	XAutoRepeatOff(display);

	printf("initializing glew\n");
	glewExperimental = GL_TRUE;
	glewInit();

	printf("creating vertex array\n");
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	printf("creating vertex buffer\n");
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
	             GL_STATIC_DRAW);

	printf("creating element buffer\n");
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
	             GL_STATIC_DRAW);

	printf("creating shader program\n");
	parse_shader("res/shaders/shader.glsl", &vs_src, &fs_src);
	shader_program = create_shader_program(vs_src, fs_src);
	glUseProgram(shader_program);
	printf("shader program created\n");

	pos_attrib = glGetAttribLocation(shader_program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE,
	                      7 * sizeof(float), 0);

	col_attrib = glGetAttribLocation(shader_program, "vcolor");
	glEnableVertexAttribArray(col_attrib);
	glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE,
	                      7 * sizeof(float), (void *)(2 * sizeof(float)));

	tex_attrib = glGetAttribLocation(shader_program, "vtexcoord");
	glEnableVertexAttribArray(tex_attrib);
	glVertexAttribPointer(tex_attrib, 2, GL_FLOAT, GL_FALSE,
	                      7 * sizeof(float), (void *)((5 * sizeof(float))));

	if (!load_tga_file(&test_image, "res/textures/test.tga"))
		die("error loading tga file: %s\n", img_strerror(img_err));

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
	             test_image.width, test_image.height,
		     0, GL_RGBA, GL_FLOAT, test_image.data);
}

void
handle_events(void)
{
	XEvent e;
	int i;

	for (;;) {
		if (!XPending(display))
			break;
		XNextEvent(display, &e);
		if (handler[e.type])
			handler[e.type](&e);
	}

	for (i = 0; i < LENGTH(keys); i++)
		if (keys[i].func && keys[i].pressed)
			keys[i].func();
}

void
render(void)
{
	matrix proj, model, camm, viewm;
	matrix rot1, cam_rot;
	vector axis1, cam_axis;
	GLint proj_uni, model_uni, view_uni;
	unsigned int width, height;
	float aspect;
	unsigned int dummy;

	float raxis1[] = { 1.0f, 1.0f, 0.0f };
	if (!create_vector(&axis1, 3))
		die("error creating vector axis1\n");
	vector_copy_data(axis1, raxis1);
	if (!normalize_vector(&axis1, axis1))
		die("error normalizing vector axis1\n");
	if (!create_simple_matrix(&rot1, 4, 4, 1.0f))
		die("error creating matrix\n");
	if (!rotate(&rot1, rot1, 0.0f, axis1))
		die("error rotating matrix rot1\n");
	model = rot1;

	current_camera = &cam;

	if (!view(&viewm))
		die("error getting view matrix");

	aspect = (float)window_attribs.width / (float)window_attribs.height;
	if (!perspective(&proj, aspect))
		die("error getting perspective");

	/* OpenGl uses column-major order (I found out the hard way) */
	if (!transpose(&model, model))
		die("error transposing model matrix\n");
	if (!transpose(&viewm, viewm))
		die("error transposing view matrix\n");
	if (!transpose(&proj, proj))
		die("error transposing projection matrix\n");

	model_uni = glGetUniformLocation(shader_program, "model");
	glUniformMatrix4fv(model_uni, 1, GL_FALSE, model.val);

	view_uni = glGetUniformLocation(shader_program, "view");
	glUniformMatrix4fv(view_uni, 1, GL_FALSE, viewm.val);

	proj_uni = glGetUniformLocation(shader_program, "proj");
	glUniformMatrix4fv(proj_uni, 1, GL_FALSE, proj.val);

	glClearColor(0.0, 0.7, 0.7, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, sizeof(elements) / sizeof(GLuint),
	               GL_UNSIGNED_INT, 0);

	glXSwapBuffers(display, window);
}

void
cleanup(void)
{
	glDeleteProgram(shader_program);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	XAutoRepeatOn(display);
	engine_destroy_window();
}

void
exit_game(void)
{
	cleanup();
	exit(EXIT_SUCCESS);
}
