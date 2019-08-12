#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out vec2 tex_coord;

void main()
{
    vec2 translation = vec2(0.0, 0.0);
    gl_Position = vec4(position + translation * vec2(2.0, -2.0), 0.0, 1.0);
    tex_coord = uv;
}

//-END_OF_VS-
#version 330 core

in vec2 tex_coord;

out vec4 color;

uniform sampler2D tex;

const float width = 0.5;
const float edge = 0.1;

void main()
{
    float distance = 1.0 - texture(tex, tex_coord).a;
    float alpha = 1.0 - smoothstep(width, width + edge, distance);

    color = vec4(vec3(1.0, 1.0, 1.0), alpha);

}