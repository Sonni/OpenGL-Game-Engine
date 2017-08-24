#ifndef GAME_ENGINE_AABB_H
#define GAME_ENGINE_AABB_H


#include "../intersect_data.h"
#include "collider.h"

class aabb : public collider
{
public:
    aabb(const vec3f& minExtents, const vec3f& maxExtents) :
            collider(collider::TYPE_AABB),
            min(minExtents),
            max(maxExtents),
            old_min(minExtents), old_max(maxExtents)
    {}
    
    intersect_data intersect_aabb(aabb &other) const;

    virtual void set_pos(const vec3f &pos) { min = old_min + pos; max = old_max + pos; }


    inline vec3f& get_min_extents()  { return min; }
    inline vec3f& get_max_extents()  { return max; }

    inline const vec3f& get_min_extents() const { return min; }
    inline const vec3f& get_max_extents() const { return max; }

private:
    vec3f min;
    vec3f max;
    vec3f old_max, old_min;
};


#endif //GAME_ENGINE_AABB_H
