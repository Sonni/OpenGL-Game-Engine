#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;

out vec2 tex_coord;
out vec3 world_pos;
out mat3 tbn_mat;
out vec4 shadow_coord;

uniform mat4 model;
uniform mat4 mvp;
uniform mat4 shadow_mvp;
uniform vec3 eye_pos;

const float shadow_distance = 80.0;
const float transition_amount = 10.0;

void main()
{
    gl_Position = mvp * vec4(position.x, position.z, position.y, 1.0);
    tex_coord = uv;
    world_pos = (model * vec4(position.x, position.z, position.y, 1.0)).xyz;

    shadow_coord = shadow_mvp * vec4(world_pos, 1.0);

    vec3 n = normalize((model * vec4(normal, 0.0)).xyz);
    vec3 t = normalize((model * vec4(tangent, 0.0)).xyz);
    t = normalize(t - dot(t, n) * n);

    vec3 biTangent = cross(t, n);
    tbn_mat = mat3(t, biTangent, n);

    float distance = length(eye_pos - world_pos.xyz);

	distance = distance - (shadow_distance - transition_amount);
	distance = distance / transition_amount;
	shadow_coord.w = clamp(1.0 - distance, 0.0, 1.0);
}

//-END_OF_VS-
#version 330 core


in vec2 tex_coord;
in vec3 world_pos;
in mat3 tbn_mat;
in vec4 shadow_coord;

out vec4 out_color;

uniform vec3 base_color;
uniform vec3 ambient_light;
uniform sampler2D sampler;
uniform sampler2D normal_map;
uniform sampler2D disp_tex;
uniform sampler2D shadow_tex;
uniform sampler2D specu_tex;

uniform float specular_intensity;
uniform float specular_power;

#include <light_h.glsl>
#include <shadow_h.glsl>

void main()
{
    float light_factor = calc_shadow(shadow_coord, shadow_tex);
    vec2 tex_coords = (tex_coord.xy + (normalize(eye_pos - world_pos) * tbn_mat).xy * (texture(disp_tex, tex_coord.xy).r * 0.03 + 0.0));

    vec4 color = vec4(base_color, 1);
    vec4 tex_color = texture(sampler, tex_coords.xy);

    if(tex_color != vec4(0,0,0,0))
        color *= tex_color;

	vec3 normal = normalize(tbn_mat * (255.0/128.0 * texture(normal_map, tex_coords).xyz - 1));


    vec4 total_light = vec4(ambient_light, 1);
    total_light += calc_light_effect(world_pos, normal, light_factor, specular_intensity, specular_power);


      /*  vec4 spec_color = texture(specu_tex, tex_coord.xy);

        if (spec_color.g > 0.5)
        {
                total_light = vec4(1.0, 1.0, 1.0, 0.6);
        } else {
                total_light = vec4(0.2, 0.2, 0.2, 1.0);

        }*/

    out_color = color * total_light;
}