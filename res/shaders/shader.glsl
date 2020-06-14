this is a simple shader

#shader vertex
#version 330 core

in vec3 position;
in vec3 vcolor;
in vec2 vtexcoord;

out vec3 fcolor;
out vec2 ftexcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform vec3 override_color;

void main()
{
	fcolor = override_color * vcolor;
	ftexcoord = vtexcoord;
	gl_Position = proj * view * model * vec4(position, 1.0);
}

#shader fragment
#version 330 core

in vec3 fcolor;
in vec2 ftexcoord;

out vec4 out_color;

uniform sampler2D tex;

void main()
{
	out_color = 0.5 * texture(tex, ftexcoord) + 0.5 * vec4(fcolor, 1.0);
	//out_color = vec4(fcolor, 1.0);
}
