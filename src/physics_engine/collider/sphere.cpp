#include "sphere.hpp"

intersect_data sphere::intersect_sphere(const sphere &other) const
{

    float radius_dist = radius + other.get_radius();
    vec3f direction = (other.get_center() - center);
    float center_dest = direction.length();
    direction /= center_dest;

    float distance = center_dest - radius_dist;

    return intersect_data(distance < 0, direction * distance);
}

void sphere::transform(const vec3f &translation)
{
    center += translation;
}