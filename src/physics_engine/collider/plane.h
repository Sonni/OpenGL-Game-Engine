#ifndef GAME_ENGINE_PLANE_H
#define GAME_ENGINE_PLANE_H


#include "../intersect_data.h"
#include "sphere.h"

class plane
{
public:

    plane(const vec3f& normal, float distance) : normal(normal), distance(distance)
    {}

    plane normalize() const;

    intersect_data intersect_sphere(const sphere &other) const;

    inline const vec3f& get_normal() const { return normal; }
    inline float get_distance() const { return distance; }

private:
    const vec3f normal;
    const float distance;
};


#endif //GAME_ENGINE_PLANE_H
