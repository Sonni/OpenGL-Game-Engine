#include "collider.hpp"
#include "sphere.hpp"
#include "aabb.hpp"

intersect_data collider::intersect(const collider &other) const
{
    if(type == TYPE_SPHERE && other.get_type() == TYPE_SPHERE)
    {
        sphere* self = (sphere*)this;
        return self->intersect_sphere((sphere &) other);
    }

    if(type == TYPE_AABB && other.get_type() == TYPE_AABB)
    {
        aabb* self = (aabb*)this;
        return self->intersect_aabb((aabb &) other);
    }

    std::cerr << "Error: Collisions not implemented between specified "
              << "colliders." << std::endl;
    exit(1);

    //Control should never reach this point
    return intersect_data(false, 0);
}