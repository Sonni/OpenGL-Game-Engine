#version 330 core

layout(location = 0) in vec3 in_position;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(in_position.x, in_position.z, in_position.y, 1.0);
}

//-END_OF_VS-
#version 330 core

out vec4 out_colour;

void main()
{
	out_colour = vec4(1.0);
}