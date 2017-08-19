#include "aabb.h"

intersect_data aabb::intersect_aabb(const aabb &other) const
{
    vec3f distances1 = other.get_min_extents() - max;
    vec3f distances2 = min - other.get_max_extents();

    vec3f distances = vec3f(distances1.max(distances2));

    float maxDistance = distances.max();

  
    return intersect_data(maxDistance < 0, distances);
}