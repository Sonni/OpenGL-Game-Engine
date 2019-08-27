#ifndef INC_3D_ENGINE_FRUSTUM_HPP
#define INC_3D_ENGINE_FRUSTUM_HPP


#include "frustum_plane.hpp"
#include "../../physics_engine/collider/aabb.hpp"
#include "../../physics_engine/collider/sphere.hpp"
#include "../../todo/simple_objects.hpp"

class frustum {
    enum {
        TOP = 0, BOTTOM,
        LEFT, RIGHT,
        NEARP, FARP
    };

public:

    enum { OUTSIDE, INTERSECT, INSIDE };

    frustum_plane m_planes[6];

    //n = near, f = far, t = top, b = bottom, r = right, l = left
    vec3f m_ntl, m_ntr, m_nbl, m_nbr, m_ftl, m_ftr, m_fbl, m_fbr;
    float m_nearD, m_farD, m_ratio, m_angle, m_tang;
    float m_nw, m_nh, m_fw, m_fh;

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

    void set_far_d(float farD) { m_farD = farD; }
    void set_near_d(float nearD) { m_nearD = nearD; }

    bool point_left_of_plane(const vec3f& point, frustum_plane plane) const {
        vec4f p(point.get_x(), point.get_y(), point.get_z(), 1.0f);
        vec4f pl(plane.m_normal.get_x(), plane.m_normal.get_y(), plane.m_normal.get_z(), plane.m_d);

        return pl.dot(p) > 0;
    }
};

#endif //INC_3D_ENGINE_FRUSTUM_HPP
