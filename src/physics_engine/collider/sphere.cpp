#include "sphere.h"

intersect_data sphere::intersect_sphere(const sphere &other) const
{

    float radiusDistance = radius + other.get_radius();
    vec3f direction = (other.get_center() - center);
    float centerDistance = direction.length();
    direction /= centerDistance;

    float distance = centerDistance - radiusDistance;

    return intersect_data(distance < 0, direction * distance);
}

void sphere::transform(const vec3f &translation)
{
    center += translation;
}