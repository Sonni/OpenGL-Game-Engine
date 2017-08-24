#ifndef INC_3D_ENGINE_LIGHTING_H
#define INC_3D_ENGINE_LIGHTING_H


#include "../../core_engine/math/vector.h"
#include "../../core_engine/math/math.h"

#define COLOR_DEPTH 256

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

    point_light(const base_light& base = base_light(), const attenuation& atten = attenuation(), const vec3f& position = vec3f()) :
            base(base),
            atten(atten),
            position(position)
    {
        float a = atten.exponent;
        float b = atten.linear;
        float c = atten.constant - COLOR_DEPTH * base.intensity * base.color.max();

        this->range = (-b + sqrtf(b*b - 4*a*c))/(2*a);
    }
};

struct spot_light
{
    point_light _point_light;
    float cutoff;
    quaternion q;

    spot_light(const point_light& _point_light, float viewAngle = ToRadians(170.0f), const quaternion& q = quaternion()) :
            _point_light(_point_light),
            cutoff((float) cos(viewAngle / 2.0f)),
            q(q)
    {}
};

#endif //INC_3D_ENGINE_LIGHTING_H
