#ifndef INC_3D_ENGINE_FRUSTUM_H
#define INC_3D_ENGINE_FRUSTUM_H


#include "frustum_plane.h"
#include "../../physics_engine/collider/aabb.h"
#include "../../physics_engine/collider/sphere.h"

class frustum {
    enum {
        TOP = 0,
        BOTTOM,
        LEFT,
        RIGHT,
        NEARP,
        FARP
    };


public:

    enum {
        OUTSIDE, INTERSECT, INSIDE
    };

    frustum_plane pl[6];

    //n = near, t = top, r = right, l = left
    vec3f ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
    float nearD, farD, ratio, angle, tang;
    float nw, nh, fw, fh;

    frustum();

    ~frustum();

    void create_frustum(float angle, float ratio, float nearD, float farD);

    void create_frustum(float nearW, float nearH, float farW, float farH, float nearD, float farD);

    void update_cam(const vec3f& pos, const vec3f& forward, const vec3f& up);

    int point_in_frustum(const vec3f& p) const;

    int sphere_in_frustum(const sphere& s) const;
    int aabb_in_frustum(const aabb& b) const;
    int collider_in_frustum(collider* coll)const {
        if (coll->get_type() == coll->TYPE_AABB)
        {
            return aabb_in_frustum(*(aabb*) coll);
        }
        if (coll->get_type() == coll->TYPE_SPHERE)
        {
            return sphere_in_frustum(*(sphere*) coll);
        }
        return -1;
    }


    void set_far_d(float farD) { this->farD = farD; }

    void set_near_d(float nearD) { this->nearD = nearD; }

    void draw_planes();
};

#endif //INC_3D_ENGINE_FRUSTUM_H
