This is a simple shader for simple meshes

#shader vertex
#version 330 core

in vec3 position;
in vec3 normal;

out vec3 fcolor;
out vec3 fnormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 color;

void main()
{
	float ambient_strength = 0.9;

	fcolor = color * ambient_strength;
	fnormal = normal;
	gl_Position = proj * view * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

in vec3 fcolor;
in vec3 fnormal;

out vec4 out_color;

void main()
{
	out_color = vec4(fcolor, 1.0);
}
