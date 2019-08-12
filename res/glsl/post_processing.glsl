#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 model;

out vec2 tex_coord;

void main()
{
    gl_Position = vec4(position.xy, 0.0, 1.0);
    tex_coord = position.xy * 0.5 + 0.5;
}

//-END_OF_VS-
#version 330 core

in vec2 tex_coord;

out vec3 color;

uniform sampler2D model_tex;
uniform vec2 blur;

void main()
{
    color = texture(model_tex, blur).rgb;
}