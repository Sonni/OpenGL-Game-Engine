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
   // pos.y = waterHeight + waveHeight(pos.x, pos.z);
	pos.y = 20;
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

precision highp float;
const vec2 iResolution = vec2(824, 600);

const int NUM_STEPS = 4;
const float PI = 3.14159;
const float EPSILON= 1e-3;
float EPSILON_NRM= 0.1 / iResolution.x;

// sea
const int ITER_GEOMETRY = 3;
const int ITER_FRAGMENT = 5;
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;
const vec3 SEA_BASE = vec3(0.1,0.19,0.22);
const vec3 SEA_WATER_COLOR = vec3(0.8,0.9,0.6);
float SEA_TIME = move_factor/15.0 * SEA_SPEED;
mat2 octave_m = mat2(1.6,1.2,-1.2,1.6);

// math
mat3 fromEuler(vec3 ang) {
	vec2 a1 = vec2(sin(ang.x),cos(ang.x));
	vec2 a2 = vec2(sin(ang.y),cos(ang.y));
	vec2 a3 = vec2(sin(ang.z),cos(ang.z));
	mat3 m;
	m[0] = vec3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = vec3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = vec3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}

float hash( vec2 p ) {
	float h = dot(p,vec2(127.1,311.7));
	return fract(sin(h)*43758.5453123);
}

float noise( in vec2 p ) {
	vec2 i = floor( p );
	vec2 f = fract( p );
	vec2 u = f*f*(3.0-2.0*f);
	return -1.0+2.0*mix( mix( hash( i + vec2(0.0,0.0) ),
	hash( i + vec2(1.0,0.0) ), u.x),
	mix( hash( i + vec2(0.0,1.0) ),
	hash( i + vec2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(vec3 n,vec3 l,float p) {
	return pow(dot(n,l) * 0.4 + 0.6,p);
}

float specular(vec3 n,vec3 l,vec3 e,float s) {
	float nrm = (s + 8.0) / (3.1415 * 8.0);
	return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
vec3 getSkyColor(vec3 e) {
	e.y = max(e.y,0.0);
	vec3 ret;
	ret.x = pow(1.0-e.y,2.0);
	ret.y = 1.0-e.y;
	ret.z = 0.6+(1.0-e.y)*0.4;
	return ret;
}

// sea
float sea_octave(vec2 uv, float choppy) {
	uv += noise(uv);
	vec2 wv = 1.0-abs(sin(uv));
	vec2 swv = abs(cos(uv));
	wv = mix(wv,swv,wv);
	return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(vec3 p) {
	float freq = SEA_FREQ;
	float amp = SEA_HEIGHT;
	float choppy = SEA_CHOPPY;
	vec2 uv = p.xz; uv.x *= 0.75;

	float d, h = 0.0;
	for(int i = 0; i < ITER_GEOMETRY; i++) {
		d = sea_octave((uv+SEA_TIME)*freq,choppy);
		d += sea_octave((uv-SEA_TIME)*freq,choppy);
		h += d * amp;
		uv *= octave_m; freq *= 1.9; amp *= 0.22;
		choppy = mix(choppy,1.0,0.2);
	}
	return p.y - h;
}

float map_detailed(vec3 p) {
	float freq = SEA_FREQ;
	float amp = SEA_HEIGHT;
	float choppy = SEA_CHOPPY;
	vec2 uv = p.xz; uv.x *= 0.75;

	float d, h = 0.0;
	for(int i = 0; i < ITER_FRAGMENT; i++) {
		d = sea_octave((uv+SEA_TIME)*freq,choppy);
		d += sea_octave((uv-SEA_TIME)*freq,choppy);
		h += d * amp;
		uv *= octave_m; freq *= 1.9; amp *= 0.22;
		choppy = mix(choppy,1.0,0.2);
	}
	return p.y - h;
}

vec3 getSeaColor(vec3 p, vec3 n, vec3 l, vec3 eye, vec3 dist) {
	float fresnel = 1.0 - max(dot(n,-eye),0.0);
	fresnel = pow(fresnel,3.0) * 0.65;

	vec3 reflected = getSkyColor(reflect(eye,n));
	vec3 refracted = SEA_BASE + diffuse(n,l,80.0) * SEA_WATER_COLOR * 0.12;

	vec3 color = mix(refracted,reflected,fresnel);

	float atten = max(1.0 - dot(dist,dist) * 0.001, 0.0);
	color += SEA_WATER_COLOR * (p.y - SEA_HEIGHT) * 0.18 * atten;

	color += vec3(specular(n,l,eye,60.0));

	return color;
}

// tracing
vec3 getNormal(vec3 p, float eps) {
	vec3 n;
	n.y = map_detailed(p);
	n.x = map_detailed(vec3(p.x+eps,p.y,p.z)) - n.y;
	n.z = map_detailed(vec3(p.x,p.y,p.z+eps)) - n.y;
	n.y = eps;
	return normalize(n);
}

float heightMapTracing(vec3 ori, vec3 dir, out vec3 p) {
	float tm = 0.0;
	float tx = 1000.0;
	float hx = map(ori + dir * tx);
	if(hx > 0.0) return tx;
	float hm = map(ori + dir * tm);
	float tmid = 0.0;
	for(int i = 0; i < NUM_STEPS; i++) {
		tmid = mix(tm,tx, hm/(hm-hx));
		p = ori + dir * tmid;
		float hmid = map(p);
		if(hmid < 0.0) {
			tx = tmid;
			hx = hmid;
		} else {
			tm = tmid;
			hm = hmid;
		}
	}
	return tmid;
}

void main()
{






	vec2 uv = tex_coord / iResolution.xy;
	uv = uv * 2.0 - 1.0;
	uv.x *= iResolution.x / iResolution.y;
	float time = move_factor/15.0 * 0.3;

	// ray
	vec3 ang = vec3(sin(time*3.0)*0.1,sin(time)*0.2+0.3,time);
	vec3 ori = vec3(0.0,3.5,time*5.0);
	vec3 dir = normalize(vec3(uv.xy,-2.0)); dir.z += length(uv) * 0.15;
	dir = normalize(dir) * fromEuler(ang);

	// tracing
	vec3 p;
	heightMapTracing(ori,dir,p);
	vec3 dist = p - ori;
	vec3 n = getNormal(p, dot(dist,dist) * EPSILON_NRM);
	vec3 light = normalize(vec3(0.0,1.0,0.8));

	// color
	vec3 colora = mix(
	getSkyColor(dir),
	getSeaColor(p,n,light,dir,dist),
	pow(smoothstep(0.0,-0.05,dir.y),0.3));

	// post
	colora = vec3(pow(colora,vec3(0.75)));

	color = vec4(colora, 1);

	/*color = mix(reflect_color, refract_color, refrac_factor);
	color = mix(color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specular_highlight, 0.0);
	color.a = clamp(water_depth/5.0, 0.0, 1.0);*/

}