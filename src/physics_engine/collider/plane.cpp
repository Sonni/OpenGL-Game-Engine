#include "plane.h"

plane plane::normalize() const
{
    float magnitude = normal.length();


    return plane(normal / magnitude, distance / magnitude);
}

intersect_data plane::intersect_sphere(const sphere &other) const
{

    float distanceFromSphereCenter = (float) fabs(normal.dot(other.get_center()) + distance);

    float distanceFromSphere = distanceFromSphereCenter - other.get_radius();

    return intersect_data(distanceFromSphere < 0, normal * distanceFromSphere);
}