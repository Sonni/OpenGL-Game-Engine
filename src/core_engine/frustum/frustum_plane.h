#ifndef INC_3D_ENGINE_FRUSTUM_PLANE_H
#define INC_3D_ENGINE_FRUSTUM_PLANE_H


#include "../math/vector.h"

class frustum_plane
{
public:

    vec3f normal, point;
    float d;


    frustum_plane(const vec3f& v1, const vec3f& v2, const vec3f& v3);
    frustum_plane(void);
    ~frustum_plane();

    void set_points(const vec3f& v1, const vec3f& v2, const vec3f& v3);
    void set_normal_and_point(const vec3f& normal, const vec3f& point);
    void set_coefficients(float a, float b, float c, float d);
    float distance(vec3f p);

    void print();
};


#endif //INC_3D_ENGINE_FRUSTUM_PLANE_H
