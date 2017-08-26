#ifndef GAME_ENGINE_VECTOR_H
#define GAME_ENGINE_VECTOR_H

#include <math.h>
#include <iostream>
#include <iomanip>

class quaternion;

template<typename T, unsigned int D>
class vec
{
public:
    vec() { }

    inline T dot(const vec<T, D> &r) const
    {
        T result = T(0);
        for(unsigned int i = 0; i < D; i++)
            result += (*this)[i] * r[i];

        return result;
    }

    inline vec<T,D> max(const vec<T,D>& r) const
    {
        vec<T,D> result;
        for(unsigned int i = 0; i < D; i++)
        {
            result[i] = values[i] > r[i] ? values[i] : r[i];
        }

        return result;
    }

    inline T max() const
    {
        T maxVal = (*this)[0];

        for(int i = 0; i < D; i++)
            if((*this)[i] > maxVal)
                maxVal = (*this)[i];

        return maxVal;
    }

    inline vec<T,D> reflect(const vec<T,D>& normal) const
    {
        return *this - (normal * (this->dot(normal) * 2));
    }

    inline T length_sq() const { return this->dot(*this); }
    inline T length() const { return sqrt(length_sq()); }
    inline vec<T,D> normalized() const { return *this/ length(); }
    inline vec<T,D> lerp(const vec<T, D> &r, T lerpFactor) const { return (r - *this) * lerpFactor + *this; }

    inline vec<T,D> interpolate(const vec<T, D> &r, float progression) const
    {
        vec<T, D> res;

        for(unsigned int i = 0; i < D; i++)
            res[i] = (*this)[i] + (r[i] - (*this)[i]) * progression;

        return res;
    }

    inline vec<T, D> operator+(const vec<T,D>& r) const
    {
        vec<T, D> result;
        for(unsigned int i = 0; i < D; i++)
            result[i] = values[i] + r[i];

        return result;
    }

    inline vec<T, D> operator-(const vec<T,D>& r) const
    {
        vec<T, D> result;
        for(unsigned int i = 0; i < D; i++)
            result[i] = values[i] - r[i];

        return result;
    }

    inline vec<T, D> operator*(const T& r) const
    {
        vec<T, D> result;
        for(unsigned int i = 0; i < D; i++)
            result[i] = values[i] * r;

        return result;
    }

    inline vec<T, D> operator/(const T& r) const
    {
        vec<T, D> result;
        for(unsigned int i = 0; i < D; i++)
            result[i] = values[i] / r;

        return result;
    }

    inline vec<T, D>& operator+=(const vec<T,D>& r)
    {
        for(unsigned int i = 0; i < D; i++)
            (*this)[i] = values[i] + r[i];

        return *this;
    }

    inline vec<T, D>& operator-=(const vec<T,D>& r)
    {
        for(unsigned int i = 0; i < D; i++)
            (*this)[i] = values[i] - r[i];

        return *this;
    }

    inline vec<T, D>& operator*=(const T& r)
    {
        for(unsigned int i = 0; i < D; i++)
            (*this)[i] = values[i] * r;

        return *this;
    }

    inline vec<T, D>& operator/=(const T& r)
    {
        for(unsigned int i = 0; i < D; i++)
            (*this)[i] = values[i] / r;

        return *this;
    }

    T& operator [] (unsigned int i) { return values[i]; }
    T operator [] (unsigned int i) const { return values[i]; }

    inline bool operator==(const vec<T,D>& r) const
    {
        for(unsigned int i = 0; i < D; i++)
            if((*this)[i] != r[i])
                return false;
        return true;
    }

    inline bool operator!=(const vec<T,D>& r) const { return !operator==(r); }

    friend std::ostream& operator<<(std::ostream& out, const vec<T,D>& r)
    {
        std::string result;
        for (unsigned int i = 0; i < D; i++)
            result += std::to_string(r.values[i]) + " ";

        out <<  result << std::endl;
        return out;
    }
protected:
private:
    T values[D];
};

template<typename T>
class vec2 : public vec<T, 2>
{
public:
    vec2() { }

    vec2(const vec<T, 2>& r)
    {
        (*this)[0] = r[0];
        (*this)[1] = r[1];
    }

    vec2(T x, T y)
    {
        (*this)[0] = x;
        (*this)[1] = y;
    }

    T cross(const vec2<T>& r) const
    {
        return get_x() * r.get_y() - get_y() * r.get_x();
    }

    inline T get_x() const { return (*this)[0]; }
    inline T get_y() const { return (*this)[1]; }

    inline void set_x(const T &x) { (*this)[0] = x; }
    inline void set_y(const T &y) { (*this)[1] = y; }

    inline void set(const T &x, const T &y) { set_x(x);
        set_y(y); }
protected:
private:
};

template<typename T>
class vec3 : public vec<T, 3>
{
public:
    vec3() { }

    vec3(const vec<T, 3>& r)
    {
        (*this)[0] = r[0];
        (*this)[1] = r[1];
        (*this)[2] = r[2];
    }

    vec3(T x, T y, T z)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    inline vec3<T> cross(const vec3<T> &r) const
    {
        T x = (*this)[1] * r[2] - (*this)[2] * r[1];
        T y = (*this)[2] * r[0] - (*this)[0] * r[2];
        T z = (*this)[0] * r[1] - (*this)[1] * r[0];

        return vec3<T>(x, y, z);
    }

    inline vec3<T> rotate(T angle, const vec3<T>& axis) const
    {
        const T sinAngle = sin(-angle);
        const T cosAngle = cos(-angle);

        return this->cross(axis * sinAngle) +        //Rotation on local X
               (*this * cosAngle) +                     //Rotation on local Z
               axis * this->dot(axis * (1 - cosAngle)); //Rotation on local Y
    }

    inline T get_x() const { return (*this)[0]; }
    inline T get_y() const { return (*this)[1]; }
    inline T get_z() const { return (*this)[2]; }

    inline void set_x(const T &x) { (*this)[0] = x; }
    inline void set_y(const T &y) { (*this)[1] = y; }
    inline void set_z(const T &z) { (*this)[2] = z; }

    inline void set(const T &x, const T &y, const T &z)
    {
        set_x(x);
        set_y(y);
        set_z(z);
    }

    inline void set(const vec3<T> &v)
    {
        set_x(v.get_x());
        set_y(v.get_y());
        set_z(v.get_z());
    }
protected:
private:
};

template<typename T>
class vec4 : public vec<T, 4>
{
public:
    vec4() { }

    vec4(const vec<T, 4>& r)
    {
        (*this)[0] = r[0];
        (*this)[1] = r[1];
        (*this)[2] = r[2];
        (*this)[3] = r[3];
    }

    vec4(T x, T y, T z, T w)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
        (*this)[3] = w;
    }

    inline T get_x() const { return (*this)[0]; }
    inline T get_y() const { return (*this)[1]; }
    inline T get_z() const { return (*this)[2]; }
    inline T get_w() const { return (*this)[3]; }

    inline void set_x(const T &x) { (*this)[0] = x; }
    inline void set_y(const T &y) { (*this)[1] = y; }
    inline void set_z(const T &z) { (*this)[2] = z; }
    inline void set_w(const T &w) { (*this)[3] = w; }

    inline void set(const T &x, const T &y, const T &z, const T &w)
    {
        set_x(x);
        set_y(y);
        set_z(z);
        set_w(w);
    }
protected:
private:
};

class vec3f : public vec3<float>
{
public:
    vec3f(float x = 0.0f, float y = 0.0f, float z = 0.0f)
    {
        (*this)[0] = x;
        (*this)[1] = y;
        (*this)[2] = z;
    }

    vec3f(const vec3<float>& r)
    {
        (*this)[0] = r[0];
        (*this)[1] = r[1];
        (*this)[2] = r[2];
    }

    inline float length() const { return sqrtf(get_x() * get_x() + get_y() * get_y() + get_z() * get_z()); }
    inline float dot(const vec3f &v) const { return get_x() * v.get_x() + get_y() * v.get_y() + get_z() * v.get_z(); }

    inline vec3f cross(const vec3f &v) const
    {
        const float _x = get_y() * v.get_z() - get_z() * v.get_y();
        const float _y = get_z() * v.get_x() - get_x() * v.get_z();
        const float _z = get_x() * v.get_y() - get_y() * v.get_x();

        return vec3f(_x, _y, _z);
    }

    inline vec3f rotate(float angle, const vec3f &axis) const
    {
        const float sin = sinf(-angle);
        const float cos = cosf(-angle);

        return this->cross(axis * sin) +        //Rotation on local X
               (*this * cos) +                     //Rotation on local Z
               axis * this->dot(axis * (1 - cos)); //Rotation on local Y
    }

    vec3f rotate(const quaternion& rotation) const;

    inline vec3f normalized() const
    {
        const float l = length();

        return vec3f(get_x() / l, get_y() / l, get_z() / l);
    }

    inline vec3f negate()
    {
        return vec3f(-get_x(), -get_y(), -get_z());
    }

    inline vec3f operator+(const vec3f& r) const { return vec3f(get_x() + r.get_x(), get_y() + r.get_y(), get_z() + r.get_z()); }
    inline vec3f operator-(const vec3f& r) const { return vec3f(get_x() - r.get_x(), get_y() - r.get_y(), get_z() - r.get_z()); }
    inline vec3f operator*(float f) const { return vec3f(get_x() * f, get_y() * f, get_z() * f); }
    inline vec3f operator/(float f) const { return vec3f(get_x() / f, get_y() / f, get_z() / f); }

    inline bool operator==(const vec3f& r) const { return get_x() == r.get_x() && get_y() == r.get_y() && get_z() == r.get_z(); }
    inline bool operator!=(const vec3f& r) const { return !operator==(r); }

    inline vec3f& operator+=(const vec3f& r)
    {
        (*this)[0] += r.get_x();
        (*this)[1] += r.get_y();
        (*this)[2] += r.get_z();

        return *this;
    }

    inline vec3f& operator-=(const vec3f& r)
    {
        (*this)[0] -= r.get_x();
        (*this)[1] -= r.get_y();
        (*this)[2] -= r.get_z();

        return *this;
    }

    inline vec3f& operator*=(float f)
    {
        (*this)[0] *= f;
        (*this)[1] *= f;
        (*this)[2] *= f;

        return *this;
    }

    inline vec3f& operator/=(float f)
    {
        (*this)[0] /= f;
        (*this)[1] /= f;
        (*this)[2] /= f;

        return *this;
    }

    inline vec3f operator*(const vec3f& r) const
    {
        return vec3f((*this)[1] * r.get_z() - (*this)[2] * r.get_y(), (*this)[2] * r.get_x() - (*this)[0] * r.get_z(), (*this)[0] * r.get_y() - (*this)[1] * r.get_x());
    }

    inline void scale(float factor) { (*this)[0] *= factor; (*this)[1] *= factor; (*this)[2] *= factor; }

    inline float get_x() const { return (*this)[0]; }
    inline float get_y() const { return (*this)[1]; }
    inline float get_z() const { return (*this)[2]; }

    inline void set_x(float x) { (*this)[0] = x; }
    inline void set_y(float y) { (*this)[1] = y; }
    inline void set_z(float z) { (*this)[2] = z; }

    inline void set(float x, float y, float z) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; }
    inline void set(const vec3f &v) {
        set_x(v.get_x());
        set_y(v.get_y());
        set_z(v.get_z()); }
private:
    //float x,y,z;
};

typedef vec2<int> vec2i;
typedef vec3<int> vec3i;
typedef vec4<int> vec4i;

typedef vec2<float> vec2f;
//typedef vec3<float> vec3f;
typedef vec4<float> vec4f;

typedef vec2<double> vec2d;
typedef vec3<double> vec3d;
typedef vec4<double> vec4d;


#endif //GAME_ENGINE_VECTOR_H
