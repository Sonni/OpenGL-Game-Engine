#include "frustum_plane.h"

frustum_plane::frustum_plane(const vec3f& v1,  const vec3f& v2,  const vec3f& v3)
{
    set_points(v1, v2, v3);
}


frustum_plane::frustum_plane() {}

frustum_plane::~frustum_plane() {}


void frustum_plane::set_points(const vec3f& v1, const vec3f& v2, const vec3f& v3)
{
    vec3f aux1, aux2;

    aux1 = v1 - v2;
    aux2 = v3 - v2;


    normal = aux2 * aux1;

    normal = normal.normalized();
    point.set(v2.get_x(), v2.get_y(), v2.get_z());
    d = -(normal.dot(point));
}

void frustum_plane::set_normal_and_point(const vec3f& normal, const vec3f& point)
{
    this->normal.set(normal.get_x(), normal.get_y(), normal.get_z());
    this->normal = this->normal.normalized();
    d = -(this->normal.dot(point));
}

void frustum_plane::set_coefficients(float a, float b, float c, float d)
{
    normal.set(a, b, c);
    float l = normal.length();
    normal = normal.normalized();

    this->d = d / l;
}


float frustum_plane::distance(vec3f p) const
{
    return (d + normal.dot(p));
}

void frustum_plane::print()
{
    //printf("Plane(");normal.print();printf("# %f)",d);
}
