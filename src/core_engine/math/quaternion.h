#ifndef GAME_ENGINE_QUATERNION_H
#define GAME_ENGINE_QUATERNION_H

#include "vector.h"
#include "matrix.h"

class quaternion : public vec4<float>
{
public:
    quaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
        (*this)[3] = w;
    }

    quaternion(const vec4<float>& r)
    {
        (*this)[0] = r[0];
        (*this)[1] = r[1];
        (*this)[2] = r[2];
        (*this)[3] = r[3];
    }

    quaternion(const vec3f& axis, float angle)
    {
        float sinHalfAngle = sinf(angle/2);
        float cosHalfAngle = cosf(angle/2);

        (*this)[0] = axis.get_x() * sinHalfAngle;
        (*this)[1] = axis.get_y() * sinHalfAngle;
        (*this)[2] = axis.get_z() * sinHalfAngle;
        (*this)[3] = cosHalfAngle;
    }

    quaternion(const mat4f& m)
    {
        float trace = m[0][0] + m[1][1] + m[2][2];

        if(trace > 0)
        {
            float s = 0.5f / sqrtf(trace + 1.0f);
            (*this)[3] = 0.25f / s;
            (*this)[0] = (m[1][2] - m[2][1]) * s;
            (*this)[1] = (m[2][0] - m[0][2]) * s;
            (*this)[2] = (m[0][1] - m[1][0]) * s;
        }
        else if(m[0][0] > m[1][1] && m[0][0] > m[2][2])
        {
            float s = 2.0f * sqrtf(1.0f + m[0][0] - m[1][1] - m[2][2]);
            (*this)[3] = (m[1][2] - m[2][1]) / s;
            (*this)[0] = 0.25f * s;
            (*this)[1] = (m[1][0] + m[0][1]) / s;
            (*this)[2] = (m[2][0] + m[0][2]) / s;
        }
        else if(m[1][1] > m[2][2])
        {
            float s = 2.0f * sqrtf(1.0f + m[1][1] - m[0][0] - m[2][2]);
            (*this)[3] = (m[2][0] - m[0][2]) / s;
            (*this)[0] = (m[1][0] + m[0][1]) / s;
            (*this)[1] = 0.25f * s;
            (*this)[2] = (m[2][1] + m[1][2]) / s;
        }
        else
        {
            float s = 2.0f * sqrtf(1.0f + m[2][2] - m[1][1] - m[0][0]);
            (*this)[3] = (m[0][1] - m[1][0]) / s;
            (*this)[0] = (m[2][0] + m[0][2]) / s;
            (*this)[1] = (m[1][2] + m[2][1]) / s;
            (*this)[2] = 0.25f * s;
        }

        float l = length();
        (*this)[3] = (*this)[3] / l;
        (*this)[0] = (*this)[0] / l;
        (*this)[1] = (*this)[1] / l;
        (*this)[2] = (*this)[2] / l;
    }

    inline quaternion n_lerp(const quaternion &r, float lerpFactor, bool shortestPath) const
    {
        quaternion correctedDest;

        if(shortestPath && this->dot(r) < 0)
            correctedDest = r * -1;
        else
            correctedDest = r;

        return quaternion(lerp(correctedDest, lerpFactor).normalized());
    }

    inline quaternion s_lerp(const quaternion &r, float lerpFactor, bool shortestPath) const
    {
        static const float EPSILON = 1e3;

        float cos = this->dot(r);
        quaternion correctedDest;

        if(shortestPath && cos < 0)
        {
            cos *= -1;
            correctedDest = r * -1;
        }
        else
            correctedDest = r;

        if(fabs(cos) > (1 - EPSILON))
            return n_lerp(correctedDest, lerpFactor, false);

        float sin = sqrtf(1.0f - cos * cos);
        float angle = (float) atan2(sin, cos);
        float invSin = 1.0f/sin;

        float srcFactor = sinf((1.0f - lerpFactor) * angle) * invSin;
        float destFactor = sinf((lerpFactor) * angle) * invSin;

        return quaternion((*this) * srcFactor + correctedDest * destFactor);
    }

    inline mat4f to_rotation_matrix() const
    {
        vec3f forward = vec3f(2.0f * (get_x() * get_z() - get_w() * get_y()), 2.0f * (get_y() * get_z() + get_w() * get_x()), 1.0f - 2.0f * (get_x() * get_x() + get_y() * get_y()));
        vec3f up = vec3f(2.0f * (get_x()* get_y() + get_w()* get_z()), 1.0f - 2.0f * (get_x()* get_x() + get_z()* get_z()), 2.0f * (get_y()* get_z() - get_w()* get_x()));
        vec3f right = vec3f(1.0f - 2.0f * (get_y()* get_y() + get_z()* get_z()), 2.0f * (get_x()* get_y() - get_w()* get_z()), 2.0f * (get_x()* get_z() + get_w()* get_y()));

        return mat4f().init_rotation_from_vectors(forward, up, right);
    }

    inline vec3f get_forward() const
    {
        return vec3f(0, 0, 1).rotate(*this);
    }

    inline vec3f get_back() const
    {
        return vec3f(0, 0, -1).rotate(*this);
    }

    inline vec3f get_up() const
    {
        return vec3f(0, 1, 0).rotate(*this);
    }

    inline vec3f get_down() const
    {
        return vec3f(0, -1, 0).rotate(*this);
    }

    inline vec3f get_right() const
    {
        return vec3f(1, 0, 0).rotate(*this);
    }

    inline vec3f get_left() const
    {
        return vec3f(-1, 0, 0).rotate(*this);
    }

    inline quaternion negate() const { return quaternion(-get_x(), -get_y(), -get_z(), get_w()); }

    inline quaternion operator*(const quaternion& r) const
    {
        const float _w = (get_w() * r.get_w()) - (get_x() * r.get_x()) - (get_y() * r.get_y()) - (get_z() * r.get_z());
        const float _x = (get_x() * r.get_w()) + (get_w() * r.get_x()) + (get_y() * r.get_z()) - (get_z() * r.get_y());
        const float _y = (get_y() * r.get_w()) + (get_w() * r.get_y()) + (get_z() * r.get_x()) - (get_x() * r.get_z());
        const float _z = (get_z() * r.get_w()) + (get_w() * r.get_z()) + (get_x() * r.get_y()) - (get_y() * r.get_x());

        return quaternion(_x, _y, _z, _w);
    }

    inline quaternion operator*(const vec3<float>& v) const
    {
        const float _w = - (get_x() * v.get_x()) - (get_y() * v.get_y()) - (get_z() * v.get_z());
        const float _x =   (get_w() * v.get_x()) + (get_y() * v.get_z()) - (get_z() * v.get_y());
        const float _y =   (get_w() * v.get_y()) + (get_z() * v.get_x()) - (get_x() * v.get_z());
        const float _z =   (get_w() * v.get_z()) + (get_x() * v.get_y()) - (get_y() * v.get_x());

        return quaternion(_x, _y, _z, _w);
    }
};
#endif //GAME_ENGINE_QUATERNION_H
