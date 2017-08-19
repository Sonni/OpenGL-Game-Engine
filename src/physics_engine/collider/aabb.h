#ifndef GAME_ENGINE_AABB_H
#define GAME_ENGINE_AABB_H


#include "../intersect_data.h"

class aabb
{
public:
    aabb(const vec3f& minExtents, const vec3f& maxExtents) :
            min(minExtents),
            max(maxExtents)
    {}
    
    intersect_data intersect_aabb(const aabb &other) const;

    inline const vec3f& get_min_extents() const { return min; }
    inline const vec3f& get_max_extents() const { return max; }
    
private:
    const vec3f min;
    const vec3f max;
};


#endif //GAME_ENGINE_AABB_H
