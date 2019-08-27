#ifndef INC_3D_ENGINE_LIGHTING_HPP
#define INC_3D_ENGINE_LIGHTING_HPP

#include "../../core_engine/math/vector.hpp"
#include "../../core_engine/math/math.hpp"

#define COLOR_DEPTH 256

struct base_light
{
    vec3f m_color;
    float m_intensity;

    base_light(const vec3f& color = vec3f(0,0,0), float intensity = 0) :
            m_color(color),
            m_intensity(intensity) {}
};

struct directional_light
{
    base_light m_base;
    vec3f m_direction;

    directional_light(const base_light& base = base_light(), vec3f direction = vec3f(0,0,0)) :
            m_base(base),
            m_direction(direction)
    {}
};

struct attenuation
{
    float m_constant;
    float m_linear;
    float m_exponent;

    attenuation(float constant = 0, float linear = 0, float exponent = 1) :
            m_constant(constant),
            m_linear(linear),
            m_exponent(exponent) {}
};

struct point_light
{
    base_light m_base;
    attenuation m_atten;
    vec3f m_position;
    float m_range;

    point_light(const base_light& base = base_light(), const attenuation& atten = attenuation(), const vec3f& position = vec3f()) :
            m_base(base),
            m_atten(atten),
            m_position(position)
    {
        float a = atten.m_exponent;
        float b = atten.m_linear;
        float c = atten.m_constant - COLOR_DEPTH * base.m_intensity * base.m_color.max();

        m_range = (-b + sqrtf(b*b - 4*a*c))/(2*a);
    }
};

struct spot_light
{
    point_light m_point_light;
    float m_cutoff;
    quaternion m_q;

    spot_light(const point_light& _point_light, float viewAngle = ToRadians(170.0f), const quaternion& q = quaternion()) :
            m_point_light(_point_light),
            m_cutoff((float) cos(viewAngle / 2.0f)),
            m_q(q)
    {}
};

#endif //INC_3D_ENGINE_LIGHTING_HPP
