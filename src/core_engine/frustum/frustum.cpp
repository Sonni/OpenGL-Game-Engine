#include "frustum.h"

#define RADIAN 180.0/3.14159265358979323846


frustum::frustum() {}

frustum::~frustum() {}

void frustum::create_frustum(float angle, float ratio, float nearD, float farD)
{
    m_ratio = ratio;
    m_angle = angle;
    m_nearD = nearD;
    m_farD = farD;



    m_tang = tanf(angle / 2) ;
    m_nh = nearD * m_tang;
    m_nw = m_nh * ratio;
    m_fh = farD  * m_tang;
    m_fw = m_fh * ratio;
}

void frustum::create_frustum(float nearW, float nearH, float farW, float farH, float nearD, float farD)
{
    m_ratio = 1;
    m_angle = 45;
    m_nearD = nearD;
    m_farD = farD;

    m_nh = nearH;
    m_nw = nearW;
    m_fh = farH;
    m_fw = farW;
}


void frustum::update_cam(const vec3f& pos, const vec3f& forward, const vec3f& up)
{
    //nc = nearCenter, fc = farCenter
    vec3f dir, nc, fc, X, Y, Z;

    Z = forward;
    Z = Z.normalized();

    X = up * Z;
    X = X.normalized();

    Y = Z * X;

    nc = pos - Z * m_nearD;
    fc = pos - Z * m_farD;

    m_ntl = nc + Y * m_nh - X * m_nw;
    m_ntr = nc + Y * m_nh + X * m_nw;
    m_nbl = nc - Y * m_nh - X * m_nw;
    m_nbr = nc - Y * m_nh + X * m_nw;

    m_ftl = fc + Y * m_fh - X * m_fw;
    m_ftr = fc + Y * m_fh + X * m_fw;
    m_fbl = fc - Y * m_fh - X * m_fw;
    m_fbr = fc - Y * m_fh + X * m_fw;

    m_planes[TOP].set_points(m_ntr, m_ntl, m_ftl);
    m_planes[BOTTOM].set_points(m_nbl, m_nbr, m_fbr);
    m_planes[LEFT].set_points(m_ntl, m_nbl, m_fbl);
    m_planes[RIGHT].set_points(m_nbr, m_ntr, m_fbr);
    m_planes[NEARP].set_points(m_ntl, m_ntr, m_nbr);
    m_planes[FARP].set_points(m_ftr, m_ftl, m_fbl);
}


int frustum::point_in_frustum(const vec3f& p) const
{
    int result = INSIDE;
    for(int i = 0; i < 6; i++)
        if (m_planes[i].distance(p) < 0)
            return OUTSIDE;
    return(result);
}


int frustum::sphere_in_frustum(const sphere& s) const
{
    int result = INSIDE;
    float distance;

    for(int i = 0; i < 6; i++) {
        distance = m_planes[i].distance(s.get_center());
        if (distance < -s.get_radius())
            return OUTSIDE;
        else if (distance < s.get_radius())
            result =  INTERSECT;
    }
    return(result);
}


int frustum::aabb_in_frustum(const aabb& b) const
{
    vec3f min = b.get_min_extents();
    vec3f max = b.get_max_extents();

    vec3f z(max.get_x(), min.get_y(), min.get_z());
    vec3f x(min.get_x(), min.get_y(), max.get_z());
    vec3f c(max.get_x(), min.get_y(), max.get_z());

    vec3f v(min.get_x(), max.get_y(), max.get_z());
    vec3f n(max.get_x(), max.get_y(), min.get_z());
    vec3f m(min.get_x(), max.get_y(), min.get_z());

    if (point_in_frustum(min) == INSIDE) return INSIDE;
    if (point_in_frustum(z) == INSIDE) return INSIDE;
    if (point_in_frustum(x) == INSIDE) return INSIDE;
    if (point_in_frustum(c) == INSIDE) return INSIDE;

    if (point_in_frustum(max) == INSIDE) return INSIDE;
    if (point_in_frustum(v) == INSIDE) return INSIDE;
    if (point_in_frustum(n) == INSIDE) return INSIDE;
    if (point_in_frustum(m) == INSIDE) return INSIDE;


    if (point_left_of_plane(b.get_min_extents(), m_planes[LEFT]) && point_left_of_plane(b.get_max_extents(), m_planes[RIGHT])) return INSIDE;
    if (point_left_of_plane(b.get_max_extents(), m_planes[LEFT]) && point_left_of_plane(b.get_min_extents(), m_planes[RIGHT])) return INSIDE;

    if (point_left_of_plane(b.get_min_extents(), m_planes[TOP]) && point_left_of_plane(b.get_max_extents(), m_planes[BOTTOM])) return INSIDE;
    if (point_left_of_plane(b.get_max_extents(), m_planes[TOP]) && point_left_of_plane(b.get_min_extents(), m_planes[BOTTOM])) return INSIDE;

    if (point_left_of_plane(b.get_min_extents(), m_planes[NEARP]) && point_left_of_plane(b.get_max_extents(), m_planes[FARP])) return INSIDE;
    if (point_left_of_plane(b.get_max_extents(), m_planes[NEARP]) && point_left_of_plane(b.get_min_extents(), m_planes[FARP])) return INSIDE;

	return OUTSIDE;
}