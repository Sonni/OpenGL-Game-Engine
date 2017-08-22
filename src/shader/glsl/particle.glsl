#version 330 core

layout(location = 0) in vec2 position;

out vec2 tex_coord;

uniform mat4 view_projection;
uniform mat4 model;

void main()
{
    tex_coord = position + vec2(0.5, 0.5);
    tex_coord.y = 1.0 - tex_coord.y;


    vec4 world_pos = model * vec4(position, 0.0, 1.0);
    gl_Position = view_projection * world_pos;
}

//-END_OF_VS-
#version 330 core

in vec2 tex_coord;

out vec4 color;

uniform sampler2D model_tex;

void main()
{
    color = texture(model_tex, tex_coord);
}