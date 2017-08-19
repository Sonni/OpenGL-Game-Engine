#ifndef GAME_ENGINE_SPHERE_H
#define GAME_ENGINE_SPHERE_H


#include "collider.h"

class sphere : public collider
{
public:
    sphere(const vec3f& center, float radius) :
            collider(collider::TYPE_SPHERE),
            center(center),
            radius(radius)
    {}

    intersect_data intersect_sphere(const sphere &other) const;
    virtual void transform(const vec3f &translation);
    virtual void set_pos(const vec3f &pos) { center = pos; }


    virtual vec3f get_center() const { return center; }
    inline float get_radius() const { return radius; }

private:
    vec3f center;
    float radius;
};


#endif //GAME_ENGINE_SPHERE_H
