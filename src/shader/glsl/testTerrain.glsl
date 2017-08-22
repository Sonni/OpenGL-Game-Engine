#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 UV;
layout(location = 2) in vec3 normal;

out vec2 tex_coord;
out vec3 normals;
out vec3 to_light_vec[4];
out vec3 to_cam_vec;
out float visibility;
out vec4 shadow_coord;
out vec2 shadow_effect_coord;
out float in_shadow;

uniform mat4 model;
uniform mat4 view_projection;
uniform mat4 view;
uniform mat4 shadow_mvp;

uniform vec3 light_pos[4];
uniform vec4 cutting_plane;
uniform vec3 eye_pos;

const float density = 0.012;
const float gradient = 5.0;
const float shadow_distance = 80.0;
const float transition_amount = 10.0;

void main()
{
	shadow_effect_coord = UV * 100;

    vec4 world_pos = model * vec4(position, 1.0);
    shadow_coord = shadow_mvp * world_pos;

	gl_ClipDistance[0] = dot(world_pos, cutting_plane);

	vec4 pos_relative_to_cam = view_projection * world_pos;
	gl_Position = pos_relative_to_cam;

    float tmp = world_pos.y - -40.0 + 1.3; // 40 is the height of the water
	if (tmp > 1)
	{
	    in_shadow = 1;
	}
	else
	{
	    in_shadow = 0.7;
	}

	tex_coord = UV;

	normals = (model * vec4(normal, 0.0)).xyz;
	for(int i = 0; i < 4; i++)
	{
		to_light_vec[i] = light_pos[i] - world_pos.xyz;
	}
	to_cam_vec = eye_pos - world_pos.xyz;

	float distance = length(pos_relative_to_cam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);

	distance = distance - (shadow_distance - transition_amount);
	distance = distance / transition_amount;
	shadow_coord.w = clamp(1.0 - distance, 0.0, 1.0);
}


//-END_OF_VS-
#version 330 core

in vec2 tex_coord;
in vec3 normals;
in vec4 shadow_coord;
in vec3 to_light_vec[4];
in vec3 to_cam_vec;
in float visibility;
in vec2 shadow_effect_coord;
in float in_shadow;

out vec4 color;

uniform sampler2D background_tex;
uniform sampler2D r_tex;
uniform sampler2D g_tex;
uniform sampler2D b_tex;
uniform sampler2D blend_tex;
uniform sampler2D shadow_tex;
uniform sampler2D shadow_tex2;

uniform vec3 light_color[4];
uniform vec3 attenuation[4];

const vec3 sky_color = vec3(0.6, 0.6, 0.6);
const float shine_damper = 1.0;
const float reflectivity = 0.0;
const int pcf_level = 4;
const float texels = (pcf_level * 2.0 + 1.0) * (pcf_level * 2.0 + 1.0);

void main()
{
    float map_size = 2048.0;
    float texel_size = 1.0 / map_size;
    float total = 0.0;

    for (int x = -pcf_level; x <= pcf_level; x++)
    {
        for (int y = -pcf_level; y <= pcf_level; y++)
        {
            float obj_nearest_light = texture(shadow_tex, shadow_coord.xy + vec2(x, y) * texel_size).r;
            if (shadow_coord.z > obj_nearest_light)
            {
                total += 1.0;
            }
        }
    }

    total /= texels;

    float light_factor = 1.0 - (total * shadow_coord.w);

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

    vec3 unit_normal = normalize(normals);
	vec3 unit_cam_vec = normalize(to_cam_vec);

	vec3 diffuse = vec3(0.0);
	vec3 specular = vec3(0.0);

	for(int i = 0; i < 4; i++)
	{
		float distance = length(to_light_vec[i]);
		float atten_factor = attenuation[i].x + (attenuation[i].y * distance) + (attenuation[i].z * distance * distance);
		vec3 unit_light_vec = normalize(to_light_vec[i]);
		float nDotl = dot(unit_normal, unit_light_vec);
		float brightness = max(nDotl, 0.0);
		vec3 light_direction = -unit_light_vec;
		vec3 reflected_light_direction = reflect(light_direction, unit_normal);
		float specular_factor = dot(reflected_light_direction, unit_cam_vec);
		specular_factor = max(specular_factor, 0.0);
		float damp_factor = pow(specular_factor, shine_damper);
		diffuse = diffuse + (brightness * light_color[i]) / atten_factor;
		specular = specular + (damp_factor * reflectivity * light_color[i]) / atten_factor;
	}
	diffuse = max(diffuse * light_factor, 0.4);

	color =  vec4(diffuse, 1.0) * total_color + vec4(specular, 1.0);
	color = mix(vec4(sky_color, 1.0), color, visibility);

}