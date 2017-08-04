#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 model;

out vec2 tex_coord;

void main()
{
    gl_Position = model * vec4(position.x, position.y, 0.0, 1.0);
    tex_coord = vec2((position.x + 1.0) / 2.0, 1 - (position.y + 1.0) / 2.0);
}

//-END_OF_VS-
#version 330 core

in vec2 tex_coord;

out vec3 color;

uniform sampler2D model_tex;

void main()
{
    color = texture(model_tex, tex_coord).rgb;
}