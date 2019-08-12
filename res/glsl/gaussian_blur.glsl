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

out vec3 out_color;

uniform sampler2D model_tex;
uniform vec2 blur;

void main()
{
    vec4 color = vec4(0.0);
    color += texture(model_tex, tex_coord + (vec2(-3.0) * blur.xy)) * (1.0/64.0);
	color += texture(model_tex, tex_coord + (vec2(-2.0) * blur.xy)) * (6.0/64.0);
	color += texture(model_tex, tex_coord + (vec2(-1.0) * blur.xy)) * (15.0/64.0);
	color += texture(model_tex, tex_coord + (vec2(0.0) * blur.xy))  * (20.0/64.0);
	color += texture(model_tex, tex_coord + (vec2(1.0) * blur.xy))  * (15.0/64.0);
	color += texture(model_tex, tex_coord + (vec2(2.0) * blur.xy))  * (6.0/64.0);
	color += texture(model_tex, tex_coord + (vec2(3.0) * blur.xy))  * (1.0/64.0);
    //color = texture(model_tex, tex_coord).rgb;
    out_color = color.xyz;
}