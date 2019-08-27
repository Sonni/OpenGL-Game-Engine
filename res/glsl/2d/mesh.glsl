#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 model;


void main()
{
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}

//-END_OF_VS-
#version 330 core


out vec4 out_color;

uniform vec4 color;

void main()
{
    out_color = color;
}