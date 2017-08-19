#ifndef GAME_ENGINE_INTERSECT_DATA_H
#define GAME_ENGINE_INTERSECT_DATA_H

#include "../core_engine/math/vector.h"

class intersect_data
{
public:

    intersect_data(const bool doesIntersect, const vec3f& direction) :
            does_intersect(doesIntersect),
            direction(direction)
    {}

    inline bool get_does_intersect() const { return does_intersect; }
    inline float get_distance()     const { return direction.Length(); }
    inline const vec3f& get_direction() const { return direction; }
private:
    const bool does_intersect;
    const vec3f direction;

};


#endif //GAME_ENGINE_INTERSECT_DATA_H


/*

    float triangleArea(vec3f point1, vec3f point2, vec3f point3)
    {

        //Heron's fomula
        float aPoint = (float) sqrt((point2.get_x() - point1.get_x()) * (point2.get_x() - point1.get_x()) + (point2.get_y() - point1.get_y()) * (point2.get_y() - point1.get_y()) + (point2.get_z() - point1.get_z()) * (point2.get_z() - point1.get_z()));
        float bPoint = (float) sqrt((point3.get_x() - point2.get_x()) * (point3.get_x() - point2.get_x()) + (point3.get_y() - point2.get_y()) * (point3.get_y() - point2.get_y()) + (point3.get_z() - point2.get_z()) * (point3.get_z() - point2.get_z()));
        float cPoint = (float) sqrt((point1.get_x() - point3.get_x()) * (point1.get_x() - point3.get_x()) + (point1.get_y() - point3.get_y()) * (point1.get_y() - point3.get_y()) + (point1.get_z() - point3.get_z()) * (point1.get_z() - point3.get_z()));

        float semiperimeter = (aPoint + bPoint + cPoint) / 2.0f;
        return ((float) sqrt(semiperimeter * ((semiperimeter - aPoint) * (semiperimeter - bPoint) * (semiperimeter - cPoint))));

    }

    bool ray_plane(float normalX, float normalY, float normalZ, float xs, float ys, float zs, float xd, float yd,
                   float zd, vec3f planePoint1, vec3f planePoint2, vec3f planePoint3, vec3f planePoint4, float *dist,
                   vec3f *point = NULL)
    {

        float a = xd * normalX + yd * normalY + zd * normalZ;
        if(a == 0)
            return false;

        float t = ((planePoint1.get_x() * normalX + planePoint1.get_y() * normalY + planePoint1.get_z() * normalZ - normalX * xs - normalY * ys - normalZ * zs) / a);
        if(t < 0)
            return false;
        float x = xs + t * xd;
        float y = ys + t * yd;
        float z = zs + t * zd;
        vec3f cp(x, y, z);
        if(abs(triangleArea(planePoint1, planePoint3, planePoint4) - triangleArea(planePoint1, planePoint4, cp) - triangleArea(planePoint1, planePoint3, cp) - triangleArea(planePoint3, planePoint4, cp)) < 0.000001 || abs(triangleArea(planePoint1, planePoint2, planePoint3) - triangleArea(planePoint1, planePoint2, cp) - triangleArea(planePoint2, planePoint3, cp) - triangleArea(planePoint1, planePoint3, cp)) < 0.000001)
        {
            if(dist != NULL)
            {
                (*dist) = t;
                if(point != NULL)
                {
                    point->set_x(x);
                    point->set_y(y);
                    point->set_z(z);
                }
            }
            return true;
        }
        return false;
    }

    bool sphere_plane(sphere& _sphere, vec3f vNormal, vec3f point1, vec3f point2, vec3f point3, vec3f point4)
    {
        float dist1 = 0, dist2 = 0;
        if(ray_plane(-vNormal.get_x(), -vNormal.get_y(), -vNormal.get_z(), _sphere.get_center().get_x(), _sphere.get_center().get_y(),
                     _sphere.get_center().get_z(), vNormal.get_x(), vNormal.get_y(), vNormal.get_z(), point1, point2, point3, point4,
                     &dist1) ||
                ray_plane(vNormal.get_x(), vNormal.get_y(), vNormal.get_z(), _sphere.get_center().get_x(), _sphere.get_center().get_y(),
                          _sphere.get_center().get_z(), -vNormal.get_x(), -vNormal.get_y(), -vNormal.get_z(), point1, point2, point3,
                          point4, &dist2))
        {
            if(dist1 > _sphere.get_radius() || dist2 > _sphere.get_radius())
                return false;


            vec3f vecDiagonal1 = point3 - point1;
            vec3f vecDiagonal2 = point4 - point2;
            vec3f vecReference1 = vecDiagonal1;
            vec3f vecReference2 = vecDiagonal2;
            vecReference1.set_y(0);
            vecReference2.set_y(0);

            if(true)
            {
                vecDiagonal1 = vecDiagonal1.normalized();
                vecDiagonal2 = vecDiagonal2.normalized();
                vecReference1 = vecReference1.normalized();
                vecReference2 = vecReference2.normalized();
                float a1 = vecDiagonal1.dot(vecReference1);
                float a2 = vecDiagonal2.dot(vecReference2);
                if(a1 <= 0.90 || a2 <= 0.90)
                {
                    if(dist1 > 0)
                    {
                        float x = _sphere.get_center().get_x() - vNormal.get_x() * (_sphere.get_radius() - dist1);
                        float y = _sphere.get_center().get_y() - vNormal.get_y() * (_sphere.get_radius() - dist1);
                        float z = _sphere.get_center().get_z() - vNormal.get_z() * (_sphere.get_radius() - dist1);
                        _sphere.set_pos(vec3f(x, y, z));
                    }
                    else
                    {
                        float x = _sphere.get_center().get_x() + vNormal.get_x() * (_sphere.get_radius() - dist2);
                        float y = _sphere.get_center().get_y() + vNormal.get_y() * (_sphere.get_radius() - dist2);
                        float z = _sphere.get_center().get_z() + vNormal.get_z() * (_sphere.get_radius() - dist2);
                        _sphere.set_pos(vec3f(x, y, z));
                    }
                }
                else
                {
                    if(dist1 > 0)
                    {
                        float x = _sphere.get_center().get_x() - 0 * (_sphere.get_radius() - dist1);
                        float y = _sphere.get_center().get_y() - 1 * (_sphere.get_radius() - dist1);
                        float z = _sphere.get_center().get_z() - 0 * (_sphere.get_radius() - dist1);
                        _sphere.set_pos(vec3f(x, y, z));
                    }
                    else
                    {
                        float x = _sphere.get_center().get_x() + 0 * (_sphere.get_radius() - dist2);
                        float y = _sphere.get_center().get_y() + 1 * (_sphere.get_radius() - dist2);
                        float z = _sphere.get_center().get_z() + 0 * (_sphere.get_radius() - dist2);
                        _sphere.set_pos(vec3f(x, y, z));
                    }
                }
            }
            else
            {
                if(dist1 > 0)
                {
                    float x = _sphere.get_center().get_x() - vNormal.get_x() * (_sphere.get_radius() - dist1);
                    float y = _sphere.get_center().get_y() - vNormal.get_y() * (_sphere.get_radius() - dist1);
                    float z = _sphere.get_center().get_z() - vNormal.get_z() * (_sphere.get_radius() - dist1);
                    _sphere.set_pos(vec3f(x, y, z));
                }
                else
                {
                    float x = _sphere.get_center().get_x() + vNormal.get_x() * (_sphere.get_radius() - dist2);
                    float y = _sphere.get_center().get_y() + vNormal.get_y() * (_sphere.get_radius() - dist2);
                    float z = _sphere.get_center().get_z() + vNormal.get_z() * (_sphere.get_radius() - dist2);
                    _sphere.set_pos(vec3f(x, y, z));
                }
            }
            return 1;
        }
        return 0;
    }
 */