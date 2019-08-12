#version 330 core

layout(location = 0) in vec3 position;

out vec3 tex_coord;

uniform mat4 view_projection;

const float scale = 120;

void main(void)
{
	gl_Position = view_projection * vec4(position * scale, 1.0);
	tex_coord = position * scale;
}

//-END_OF_VS-
#version 330 core

in vec3 tex_coord;

out vec4 color;

uniform samplerCube cube_map;

const vec3 sky_color = vec3(0.6, 0.6, 0.6);
const float lowerLimit = 0.0;
const float upperLimit = 30.0;

void main(void)
{
    vec4 final_color = texture(cube_map, tex_coord);

    float factor = (tex_coord.y - lowerLimit) / (upperLimit - lowerLimit);
    factor = clamp(factor, 0.0, 1.0);

    color = mix(vec4(sky_color, 1.0), final_color, factor);
}