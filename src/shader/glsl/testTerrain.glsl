#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out vec3 world_pos;
out vec2 tex_coord;
out vec3 normals;

out vec4 shadow_coord;
out vec2 shadow_effect_coord;
out float in_shadow;

uniform mat4 model;
uniform mat4 view_projection;
uniform mat4 shadow_mvp;

uniform vec4 cutting_plane;

const float density = 0.012;
const float gradient = 5.0;
const float shadow_distance = 80.0;
const float transition_amount = 10.0;

void main()
{
	shadow_effect_coord = uv * 100;

    vec4 world_position = model * vec4(position, 1.0);
    shadow_coord = shadow_mvp * world_position;

	world_pos = world_position.xyz;
	tex_coord = uv;
    normals = (model * vec4(normal, 0.0)).xyz;

	gl_ClipDistance[0] = dot(world_position, cutting_plane);

	vec4 pos_relative_to_cam = view_projection * world_position;
	gl_Position = pos_relative_to_cam;


    float tmp = world_position.y - -40.0 + 1.3; // 40 is the height of the water
	if (tmp > 1) { in_shadow = 1; }
	else { in_shadow = 0.7; }
}


//-END_OF_VS-
#version 330 core

in vec3 world_pos;
in vec2 tex_coord;
in vec3 normals;
in vec4 shadow_coord;

in vec2 shadow_effect_coord;
in float in_shadow;

out vec4 out_color;

uniform sampler2D background_tex;
uniform sampler2D r_tex;
uniform sampler2D g_tex;
uniform sampler2D b_tex;
uniform sampler2D blend_tex;
uniform sampler2D shadow_tex;
uniform sampler2D shadow_tex2;

const vec3 sky_color = vec3(0.6, 0.6, 0.6);


uniform vec3 ambient_light;

#include <light_h.glsl>
#include <shadow_h.glsl>

void main()
{
    float light_factor = calc_shadow(shadow_coord, shadow_tex);

    vec4 blend_tex_color = texture(blend_tex, tex_coord);

	float back_tex_amount = 1 - (blend_tex_color.r + blend_tex_color.g + blend_tex_color.b);
	vec2 tiled_coord = tex_coord * 30.0;
	vec4 back_color = texture(background_tex, tiled_coord) * back_tex_amount;
	vec4 r_color = texture(r_tex,tiled_coord) * blend_tex_color.r;
	vec4 g_color = texture(g_tex,tiled_coord) * blend_tex_color.g;
	vec4 b_color = texture(b_tex,tiled_coord) * blend_tex_color.b;

	vec4 total_color = back_color + r_color + g_color + b_color;

    vec4 shadows = texture(shadow_tex2, shadow_effect_coord * 0.5);
	total_color = mix(shadows, total_color, in_shadow);

    vec4 total_light = vec4(ambient_light, 1);
    total_light += calc_light_effect(world_pos, normals, light_factor, 0.0, 0.0);

    out_color = total_color * total_light;
}