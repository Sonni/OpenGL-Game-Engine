#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 view_projection;
uniform mat4 model;

void main()
{
    vec4 world_pos = model * vec4(position, 1.0);
    gl_Position = view_projection * world_pos;
}

//-END_OF_VS-
#version 330 core

out vec4 color;

void main()
{
    color = vec4(1.0, 1.0, 1.0, 0.5);
}