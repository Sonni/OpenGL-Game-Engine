#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;


out vec2 texCoord0;
out vec3 worldPos0;
out mat3 tbnMatrix;
out vec4 shadow_coord;


uniform mat4 transform;
uniform mat4 transformProjected;
uniform mat4 shadow_mvp;


void main()
{
    gl_Position = transformProjected * vec4(position.x, position.z, position.y, 1.0);
    texCoord0 = texCoord;
    worldPos0 = (transform * vec4(position.x, position.z, position.y, 1.0)).xyz;

    shadow_coord = shadow_mvp * vec4(worldPos0, 1.0);


    vec3 n = normalize((transform * vec4(normal, 0.0)).xyz);
    vec3 t = normalize((transform * vec4(tangent, 0.0)).xyz);
    t = normalize(t - dot(t, n) * n);

    vec3 biTangent = cross(t, n);
    tbnMatrix = mat3(t, biTangent, n);
}

//-END_OF_VS-
#version 330 core

const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 1;
const int pcf_level = 3;
const float texels = (pcf_level * 2.0 + 1.0) * (pcf_level * 2.0 + 1.0);
const float map_size = 2048.0;

in vec2 texCoord0;
in vec3 worldPos0;
in mat3 tbnMatrix;
in vec4 shadow_coord;



out vec4 fragColor;

struct BaseLight
{
    vec3 color;
    float intensity;
};

struct DirectionalLight
{
    BaseLight base;
    vec3 direction;
};

struct Attenuation
{
    float constant;
    float linear;
    float exponent;
};

struct PointLight
{
    BaseLight base;
    Attenuation atten;
    vec3 position;
    float range;
};

struct SpotLight
{
    PointLight pointLight;
    vec3 direction;
    float cutoff;
};

uniform vec3 baseColor;
uniform vec3 eyePos;
uniform vec3 ambientLight;
uniform sampler2D sampler;
uniform sampler2D normal_map;
uniform sampler2D disp_tex;
uniform sampler2D shadow_tex;


uniform float specularIntensity;
uniform float specularPower;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec4 calcLight(BaseLight base, vec3 direction, vec3 normal)
{
    float diffuseFactor = dot(normal, -direction);

    vec4 diffuseColor = vec4(0,0,0,0);
    vec4 specularColor = vec4(0,0,0,0);

    if(diffuseFactor > 0)
    {
        diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFactor;

        vec3 directionToEye = normalize(eyePos - worldPos0);
        vec3 reflectDirection = normalize(reflect(direction, normal));

        float specularFactor = dot(directionToEye, reflectDirection);
        specularFactor = pow(specularFactor, specularPower);

        if(specularFactor > 0)
        {
            specularColor = vec4(base.color, 1.0) * specularIntensity * specularFactor;
        }
    }

    return diffuseColor + specularColor;
}

vec4 calcDirectionalLight(DirectionalLight directionalLight, vec3 normal)
{
    return calcLight(directionalLight.base, -directionalLight.direction, normal);
}

vec4 calcPointLight(PointLight pointLight, vec3 normal)
{
    vec3 lightDirection = worldPos0 - pointLight.position;
    float distanceToPoint = length(lightDirection);

    if(distanceToPoint > pointLight.range)
        return vec4(0,0,0,0);

    lightDirection = normalize(lightDirection);

    vec4 color = calcLight(pointLight.base, lightDirection, normal);

    float attenuation = pointLight.atten.constant +
                         pointLight.atten.linear * distanceToPoint +
                         pointLight.atten.exponent * distanceToPoint * distanceToPoint +
                         0.0001;

    return color / attenuation;
}

vec4 calcSpotLight(SpotLight spotLight, vec3 normal)
{
    vec3 lightDirection = normalize(worldPos0 - spotLight.pointLight.position);
    float spotFactor = dot(lightDirection, spotLight.direction);

    vec4 color = vec4(0,0,0,0);

    if(spotFactor > spotLight.cutoff)
    {
        color = calcPointLight(spotLight.pointLight, normal) *
                (1.0 - (1.0 - spotFactor)/(1.0 - spotLight.cutoff));
    }

    return color;
}

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

    vec2 tex_coords = (texCoord0.xy + (normalize(eyePos - worldPos0) * tbnMatrix).xy * (texture(disp_tex, texCoord0.xy).r * 0.03 + 0.0));
    vec4 totalLight = vec4(ambientLight,1);
    vec4 color = vec4(baseColor, 1);
    vec4 textureColor = texture(sampler, tex_coords.xy);

    if(textureColor != vec4(0,0,0,0))
        color *= textureColor;

	vec3 normal = normalize(tbnMatrix * (255.0/128.0 * texture(normal_map, tex_coords).xyz - 1));


    totalLight += calcDirectionalLight(directionalLight, normal);

    for(int i = 0; i < MAX_POINT_LIGHTS; i++)
        if(pointLights[i].base.intensity > 0)
            totalLight += calcPointLight(pointLights[i], normal);

    for(int i = 0; i < MAX_SPOT_LIGHTS; i++)
        if(spotLights[i].pointLight.base.intensity > 0)
            totalLight += calcSpotLight(spotLights[i], normal);

    fragColor = color * max(totalLight * light_factor, 0.4);
}