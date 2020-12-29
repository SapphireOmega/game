This is a simple shader for simple meshes

#shader vertex
#version 330 core

in vec3 pos;
in vec3 norm;

out vec3 fpos;
out vec3 fnorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main()
{
	fpos = vec3(model * vec4(pos, 1.0));
	fnorm = mat3(transpose(inverse(model))) * norm;

	gl_Position = proj * view * model * vec4(pos, 1.0);
}

#shader fragment
#version 330 core

in vec3 fpos;
in vec3 fnorm;

out vec4 out_col;

uniform vec3 light_pos;
uniform vec3 light_col;
uniform vec3 obj_col;

void main()
{
	float ambient_strength = 0.1;
	vec3 ambient = ambient_strength * light_col;

	vec3 norm = normalize(fnorm);
	vec3 light_dir = normalize(light_pos - fpos);
	float diff = max(dot(norm, light_dir), 0.0);
	vec3 diffuse = diff * light_col;

	out_col = vec4((ambient + diffuse) * obj_col, 1.0);
}
