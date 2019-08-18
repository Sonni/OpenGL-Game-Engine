#version 330 core

layout(location = 0) in vec3 position;


uniform mat4 model;
uniform mat4 mvp;

uniform vec3 cam_pos;

out vec2 tex_coord;
out vec4 clip_space;
out vec3 to_cam_vec;
out vec3 from_light_vec;

const float density = 0.012;
const float gradient = 5.0;
const vec3 light_pos = vec3(1000.0, 1000.0, -1000.0);
const float tiling = 0.2;

const float pi = 3.14159;

const float waterHeight = 50.0;
const float time = 0.0000;

uniform int numWaves;
uniform float amplitude[4];
uniform float wavelength[4];
uniform float speed[4];
uniform vec2 direction[4];
uniform float move_factor;



float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    return amplitude[i] * sin(theta * frequency + move_factor * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
    for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
    return height;
}

float dWavedx(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].x * frequency;
    return A * cos(theta * frequency + move_factor * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*pi/wavelength[i];
    float phase = speed[i] * frequency;
    float theta = dot(direction[i], vec2(x, y));
    float A = amplitude[i] * direction[i].y * frequency;
    return A * cos(theta * frequency + move_factor * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
    vec3 n = vec3(-dx, -dy, 1.0);
    return normalize(n);
}

void main()
{

vec4 pos = vec4(position, 1.0);
    pos.y = waterHeight + waveHeight(pos.x, pos.z);

    vec4 world_pos = model * pos;
    clip_space =  mvp * vec4(pos.x, pos.y, pos.z, 1.0);


    gl_Position = clip_space;

    tex_coord = vec2(position.x / 2.0 + 5.0, position.z / 2.0 + 0.5) * tiling;
    to_cam_vec = cam_pos - world_pos.xyz;
    from_light_vec = world_pos.xyz - light_pos;
}


//-END_OF_VS-
#version 330 core

in vec4 clip_space;
in vec2 tex_coord;
in vec3 to_cam_vec;
in vec3 from_light_vec;

out vec4 color;

uniform sampler2D reflection_tex;
uniform sampler2D refraction_tex;
uniform sampler2D dudv_tex;
uniform sampler2D normal_tex;
uniform sampler2D depth_tex;
uniform float move_factor;

const vec3 light_color = vec3(1.1, 1.1, 1.1);
const float wave_strength = 0.1;
const float shine_damper = 20.0;
const float reflectivity = 0.5;

void main()
{
	vec2 ndc = (clip_space.xy/clip_space.w)/2.0 + 0.5;
	vec2 refract_tex_coord = vec2(ndc.x, ndc.y);
	vec2 reflect_tex_coord = vec2(ndc.x, -ndc.y);

	float near = 0.1;
	float far = 1000.0;

	float depth = texture(depth_tex, refract_tex_coord).r;
	float floor_dist = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));

	depth = gl_FragCoord.z;
	float water_dist = 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
	float water_depth = floor_dist - water_dist;

	vec2 distorted_tex_coord = texture(dudv_tex, vec2(tex_coord.x + move_factor/15, tex_coord.y)).rg*0.1;
	distorted_tex_coord = tex_coord + vec2(distorted_tex_coord.x, distorted_tex_coord.y+move_factor/15);
	vec2 total_distortion = (texture(dudv_tex, distorted_tex_coord).rg * 2.0 - 1.0) * wave_strength * clamp(water_depth/20.0, 0.0, 1.0);

	refract_tex_coord += total_distortion;
	refract_tex_coord = clamp(refract_tex_coord, 0.001, 0.999);

	reflect_tex_coord += total_distortion;
	reflect_tex_coord.x = clamp(reflect_tex_coord.x, 0.001, 0.999);
	reflect_tex_coord.y = clamp(reflect_tex_coord.y, -0.999, -0.001);

	vec4 reflect_color = texture(reflection_tex, reflect_tex_coord);
	vec4 refract_color = texture(refraction_tex, refract_tex_coord);

	vec4 normal_tex_color = texture(normal_tex, distorted_tex_coord);
	vec3 normal = vec3(normal_tex_color.r * 2.0 - 1.0, normal_tex_color.b*3.0, normal_tex_color.g * 2.0 - 1.0);
	normal = normalize(normal);

	vec3 view_vec = normalize(to_cam_vec);
	float refrac_factor = dot(view_vec, normal);
	refrac_factor = pow(refrac_factor, 0.5);
	refrac_factor = clamp(refrac_factor, 0.0, 1.0);

	vec3 reflected_light = reflect(normalize(from_light_vec), normal);
	float specular = max(dot(reflected_light, view_vec), 0.0);
	specular = pow(specular, shine_damper);
	vec3 specular_highlight = light_color * specular * reflectivity * clamp(water_depth/5.0, 0.0, 1.0);


	color = mix(reflect_color, refract_color, refrac_factor);
	color = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specular_highlight, 0.0);
	color.a = clamp(water_depth/5.0, 0.0, 1.0);
}