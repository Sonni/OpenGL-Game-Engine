const int MAX_POINT_LIGHTS = 1;
const int MAX_SPOT_LIGHTS = 1;

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



uniform vec3 eye_pos;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];

vec4 calcLight(BaseLight base, vec3 direction, vec3 world_pos, vec3 normal, float light_factor, float specular_intensity, float specular_power)
{
    float diffuseFactor = dot(normal, -direction);

    vec4 diffuseColor = vec4(0,0,0,0);
    vec4 specularColor = vec4(0,0,0,0);

    if(diffuseFactor > 0)
    {
        diffuseColor = vec4(base.color, 1.0) * base.intensity * diffuseFactor;

        vec3 directionToEye = normalize(eye_pos - world_pos);
        vec3 reflectDirection = normalize(reflect(direction, normal));

        float specularFactor = dot(directionToEye, reflectDirection);
        specularFactor = pow(specularFactor, specular_power);

        if(specularFactor > 0)
        {
            specularColor = vec4(base.color, 1.0) * specular_intensity * specularFactor;
        }
    }

    return max(diffuseColor * light_factor, 0.4) + specularColor;
}


vec4 calcDirectionalLight(DirectionalLight directionalLight, vec3 world_pos, vec3 normal, float light_factor, float specular_intensity, float specular_power)
{
    return calcLight(directionalLight.base, -directionalLight.direction, world_pos, normal, light_factor, specular_intensity, specular_power);
}

vec4 calcPointLight(PointLight pointLight, vec3 world_pos, vec3 normal, float light_factor, float specular_intensity, float specular_power)
{
    vec3 lightDirection = world_pos - pointLight.position;
    float distanceToPoint = length(lightDirection);

    if(distanceToPoint > pointLight.range)
        return vec4(0,0,0,0);

    lightDirection = normalize(lightDirection);

    vec4 color = calcLight(pointLight.base, lightDirection, world_pos, normal, light_factor, specular_intensity, specular_power);

    float attenuation = pointLight.atten.constant +
                         pointLight.atten.linear * distanceToPoint +
                         pointLight.atten.exponent * distanceToPoint * distanceToPoint +
                         0.0001;

    return color / attenuation;
}

vec4 calcSpotLight(SpotLight spotLight, vec3 world_pos, vec3 normal, float light_factor, float specular_intensity, float specular_power)
{
    vec3 lightDirection = normalize(world_pos - spotLight.pointLight.position);
    float spotFactor = dot(lightDirection, spotLight.direction);

    vec4 color = vec4(0, 0, 0, 0);

    if(spotFactor > spotLight.cutoff)
    {
        color = calcPointLight(spotLight.pointLight, world_pos, normal, light_factor, specular_intensity, specular_power) * (1.0 - (1.0 - spotFactor) / (1.0 - spotLight.cutoff));
    }

    return color;
}

vec4 calc_light_effect(vec3 world_pos, vec3 normal, float light_factor, float specular_intensity, float specular_power)
{
    vec4 totalLight = calcDirectionalLight(directionalLight, world_pos, normal, light_factor, specular_intensity, specular_power);

          for(int i = 0; i < MAX_POINT_LIGHTS; i++)
              if(pointLights[i].base.intensity > 0)
                  totalLight += calcPointLight(pointLights[i], world_pos, normal, 1, specular_intensity, specular_power);

          for(int i = 0; i < MAX_SPOT_LIGHTS; i++)
              if(spotLights[i].pointLight.base.intensity > 0)
                  totalLight += calcSpotLight(spotLights[i], world_pos, normal, 1, specular_intensity, specular_power);

    return totalLight;
}