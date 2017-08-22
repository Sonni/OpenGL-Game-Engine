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

uniform mat4 model;
uniform mat4 view_projection;
uniform mat4 inverted_view;
uniform vec3 light_pos[4];
uniform mat4 shadow_mvp;

const float density = 0.012;
const float gradient = 5.0;

uniform vec4 plane;

void main()
{
    vec4 world_pos = model * vec4(position.x, position.z, position.y, 1.0);
    shadow_coord = shadow_mvp * world_pos;

	//gl_ClipDistance[0] = dot(world_pos, plane);

	vec4 pos_relative_to_cam = view_projection * world_pos;
	gl_Position = view_projection * world_pos;
	tex_coord = UV;

	vec3 actualNormal = normal;

	normals = (model * vec4(actualNormal,0.0)).xyz;
	for(int i = 0; i < 4; i++)
	{
		to_light_vec[i] = light_pos[i] - world_pos.xyz;
	}
	to_cam_vec = (inverted_view * vec4(0.0, 0.0, 0.0, 1.0)).xyz - world_pos.xyz;

	float distance = length(pos_relative_to_cam.xyz);
	visibility = exp(-pow((distance * density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}



//-END_OF_VS-
#version 330 core

in vec2 tex_coord;
in vec3 normals;
in vec3 to_light_vec[4];
in vec3 to_cam_vec;
in float visibility;
in vec4 shadow_coord;

out vec4 color;

uniform sampler2D model_tex;
uniform vec3 light_color[4];
uniform vec3 attenuation[4];
uniform sampler2D shadow_tex;

const vec3 sky_color = vec3(0.6, 0.6, 0.6);
const float shine_damper = 1.0;
const float reflectivity = 0.0;
const int pcf_level = 3;
const float texels = (pcf_level * 2.0 + 1.0) * (pcf_level * 2.0 + 1.0);
const float map_size = 2048.0;

void main()
{
    float texel_size = 1.0 / map_size;
    float total = 0.0;

    for (int x = -pcf_level; x <= pcf_level; x++)
    {
        for (int y = -pcf_level; y <= pcf_level; y++)
        {
            float obj_nearest_light = texture(shadow_tex, shadow_coord.xy + vec2(x, y) * texel_size).r;
            if (shadow_coord.z > obj_nearest_light + 0.01)
            {
                total += 1.0;
            }
        }
    }

    total /= texels;
    float light_factor = 1.0 - (total * shadow_coord.w);

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

	vec4 texture_color = texture(model_tex, tex_coord);
	if(texture_color.a < 0.5)
	{
		discard;
	}

	color = vec4(diffuse, 1.0) * texture_color + vec4(specular, 1.0);
	color = mix(vec4(sky_color, 1.0), color, visibility);
}