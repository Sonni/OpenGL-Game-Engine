#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in mat4 model;
layout(location = 5) in vec4 tex_offset;
layout(location = 6) in float blend_factor;

out vec2 tex_coord1;
out vec2 tex_coord2;
out float blend;

uniform mat4 view_projection;

uniform float num_rows;

void main()
{
    vec2 tex_coord = position + vec2(0.5, 0.5);
    tex_coord.y = 1.0 - tex_coord.y;
    tex_coord /= num_rows;
    tex_coord1 = tex_coord + tex_offset.xy;
    tex_coord2 = tex_coord + tex_offset.zw;
    blend = blend_factor;


    vec4 world_pos = model * vec4(position, 0.0, 1.0);
    gl_Position = view_projection * world_pos;
}

//-END_OF_VS-
#version 330 core

in vec2 tex_coord1;
in vec2 tex_coord2;
in float blend;

out vec4 color;

uniform sampler2D model_tex;

void main()
{
    color = mix(texture(model_tex, tex_coord1), texture(model_tex, tex_coord2), blend);
}