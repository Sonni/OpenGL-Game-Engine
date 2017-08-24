#include "frustum.h"

#define RADIAN 180.0/3.14159265358979323846


frustum::frustum() {}

frustum::~frustum() {}

void frustum::create_frustum(float angle, float ratio, float nearD, float farD)
{
    this->ratio = ratio;
    this->angle = angle;
    this->nearD = nearD;
    this->farD = farD;

    tang = (float)tan(angle * RADIAN * 0.5) ;
    nh = nearD * tang;
    nw = nh * ratio;
    fh = farD  * tang;
    fw = fh * ratio;
}

void frustum::create_frustum(float nearW, float nearH, float farW, float farH, float nearD, float farD)
{
    this->ratio = 1;
    this->angle = 45;
    this->nearD = nearD;
    this->farD = farD;

    nh = nearH;
    nw = nearW;
    fh = farH;
    fw = farW;
}


void frustum::update_cam(const vec3f& pos, const vec3f& forward, const vec3f& up)
{
    //nc = nearCenter, fc = farCenter
    vec3f dir, nc, fc, X, Y, Z;

    dir = pos;
    dir = dir.normalized();

    Z = forward;
    Z = Z.normalized();


    X = up * Z;
    X = X.normalized();

    Y = Z * X;


    nc = pos - Z * nearD;
    fc = pos - Z * farD;


    ntl = nc + Y * nh - X * nw;
    ntr = nc + Y * nh + X * nw;
    nbl = nc - Y * nh - X * nw;
    nbr = nc - Y * nh + X * nw;

    ftl = fc + Y * fh - X * fw;
    ftr = fc + Y * fh + X * fw;
    fbl = fc - Y * fh - X * fw;
    fbr = fc - Y * fh + X * fw;

    pl[TOP].set_points(ntr, ntl, ftl);
    pl[BOTTOM].set_points(nbl, nbr, fbr);
    pl[LEFT].set_points(ntl, nbl, fbl);
    pl[RIGHT].set_points(nbr, ntr, fbr);
    pl[NEARP].set_points(ntl, ntr, nbr);
    pl[FARP].set_points(ftr, ftl, fbl);
}


int frustum::point_in_frustum(const vec3f& p) const
{
    int result = INSIDE;
    for(int i = 0; i < 6; i++)
        if (pl[i].distance(p) < 0)
            return OUTSIDE;
    return(result);
}


int frustum::sphere_in_frustum(const sphere& s) const
{
    int result = INSIDE;
    float distance;

    for(int i = 0; i < 6; i++) {
        distance = pl[i].distance(s.get_center());
        if (distance < -s.get_radius())
            return OUTSIDE;
        else if (distance < s.get_radius())
            result =  INTERSECT;
    }
    return(result);
}


int frustum::aabb_in_frustum(const aabb& b) const
{

	int result = OUTSIDE;
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


	return(result);
}

void frustum::draw_planes()
{
    /*
    glBegin(GL_QUADS);

    //top plane
    glVertex3f(ntr.get_x(), ntr.get_y(), ntr.get_z());
    glVertex3f(ntl.get_x(), ntl.get_y(), ntl.get_z());
    glVertex3f(ftl.get_x(), ftl.get_y(), ftl.get_z());

    //bottom plane
    glVertex3f(nbl.get_x(), nbl.get_y(), nbl.get_z());
    glVertex3f(nbr.get_x(), nbr.get_y(), nbr.get_z());
    glVertex3f(fbr.get_x(), fbr.get_y(), fbr.get_z());

    //left plane
    glVertex3f(ntl.get_x(), ntl.get_y(), ntl.get_z());
    glVertex3f(nbl.get_x(), nbl.get_y(), nbl.get_z());
    glVertex3f(fbl.get_x(), fbl.get_y(), fbl.get_z());

    // right plane
    glVertex3f(nbr.get_x(), nbr.get_y(), nbr.get_z());
    glVertex3f(ntr.get_x(), ntr.get_y(), ntr.get_z());
    glVertex3f(fbr.get_x(), fbr.get_y(), fbr.get_z());

    //near plane
    glVertex3f(ntl.get_x(), ntl.get_y(), ntl.get_z());
    glVertex3f(ntr.get_x(), ntr.get_y(), ntr.get_z());
    glVertex3f(nbr.get_x(), nbr.get_y(), nbr.get_z());

    //far plane
    glVertex3f(ftr.get_x(), ftr.get_y(), ftr.get_z());
    glVertex3f(ftl.get_x(), ftl.get_y(), ftl.get_z());
    glVertex3f(fbl.get_x(), fbl.get_y(), fbl.get_z());

    glEnd();*/
}