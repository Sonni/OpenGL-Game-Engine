#version 330 core

const int MAX_JOINTS = 50;
const int MAX_WEIGHTS = 3;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normals;
layout(location = 3) in ivec3 joints_id;
layout(location = 4) in vec3 weights;

out vec3 world_pos;
out vec2 tex_coord;
out vec3 normal;
out vec4 shadow_coord;

uniform mat4 model;
uniform mat4 joint_mat[MAX_JOINTS];
uniform mat4 mvp;
uniform mat4 shadow_mvp;
uniform vec3 eye_pos;

const float shadow_distance = 80.0;
const float transition_amount = 10.0;


void main()
{
    vec4 new_pos = vec4(0.0);
   	vec4 new_normal = vec4(0.0);

   	for(int i = 0; i < MAX_WEIGHTS; i++)
   	{
   		new_pos += joint_mat[joints_id[i]] * vec4(position, 1.0) * weights[i];
   		new_normal += joint_mat[joints_id[i]] * vec4(normals, 0.0) * weights[i];
   	}

   	gl_Position = mvp * new_pos;
   	tex_coord = uv;
   	normal = new_normal.xyz;

   	world_pos = (model * new_pos).xyz;
   	shadow_coord = shadow_mvp * vec4(world_pos, 1.0);

    float distance = length(eye_pos - world_pos.xyz);
	distance = distance - (shadow_distance - transition_amount);
	distance = distance / transition_amount;
	shadow_coord.w = clamp(1.0 - distance, 0.0, 1.0);

}

//-END_OF_VS-
#version 330 core

in vec3 world_pos;
in vec2 tex_coord;
in vec3 normal;
in vec4 shadow_coord;


out vec4 out_color;

uniform sampler2D model_tex;

uniform vec3 base_color;
uniform vec3 ambient_light;
uniform sampler2D shadow_tex;

uniform float specular_intensity;
uniform float specular_power;

#include <light_h.glsl>
#include <shadow_h.glsl>

void main()
{
    float light_factor = calc_shadow(shadow_coord, shadow_tex);

    vec4 color = vec4(base_color, 1);
    vec4 tex_color = texture(model_tex, tex_coord.xy);

    if(tex_color != vec4(0,0,0,0))
        color *= tex_color;

    vec4 total_light = vec4(ambient_light, 1);
    total_light += calc_light_effect(world_pos, normal, light_factor, specular_intensity, specular_power);

    out_color = color * total_light;
}