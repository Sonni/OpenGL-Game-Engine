#ifndef INC_3D_ENGINE_LIGHTING_H
#define INC_3D_ENGINE_LIGHTING_H


#include "../core_engine/math/vector.h"

struct base_light
{
    vec3f color;
    float intensity;

    base_light(const vec3f& color = vec3f(0,0,0), float intensity = 0) :
            color(color),
            intensity(intensity) {}
};

struct directional_light
{
    base_light base;
    vec3f direction;

    directional_light(const base_light& base = base_light(), vec3f direction = vec3f(0,0,0)) :
            base(base),
            direction(direction)
    {}
};

struct attenuation
{
    float constant;
    float linear;
    float exponent;

    attenuation(float constant = 0, float linear = 0, float exponent = 1) :
            constant(constant),
            linear(linear),
            exponent(exponent) {}
};

struct point_light
{
    base_light base;
    attenuation atten;
    vec3f position;
    float range;

    point_light(const base_light& base = base_light(), const attenuation& atten = attenuation(), const vec3f& position = vec3f(), float range = 0) :
            base(base),
            atten(atten),
            position(position),
            range(range)
    {}
};

struct spot_light
{
    point_light _point_light;
    vec3f direction;
    float cutoff;

    spot_light(const point_light& _point_light = point_light(), const vec3f& direction = vec3f(), float cutoff = 0) :
            _point_light(_point_light),
            direction(direction),
            cutoff(cutoff)
    {}
};

#endif //INC_3D_ENGINE_LIGHTING_H
