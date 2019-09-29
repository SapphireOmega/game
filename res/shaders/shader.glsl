this is a simple shader

#shader vertex
#version 330 core

in vec2 pos;

void main()
{
	gl_Position = vec4(pos, 0.0, 1.0);
}

#shader fragment
#version 330 core

out vec4 out_color;

void main()
{
	out_color = vec4(1.0, 1.0, 1.0, 1.0);
}
