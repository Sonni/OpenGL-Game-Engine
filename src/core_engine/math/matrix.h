#ifndef GAME_ENGINE_MATRIX_H
#define GAME_ENGINE_MATRIX_H

#include "vector.h"


template<typename T, unsigned int D>
class mat
{
public:
    inline mat<T, D> init_identity()
    {
        for(unsigned int i = 0; i < D; i++)
        {
            for(unsigned int j = 0; j < D; j++)
            {
                if(i == j)
                    m[i][j] = T(1);
                else
                    m[i][j] = T(0);
            }
        }

        return *this;
    }

    inline mat<T, D> init_scale(const vec<T, D - 1> &r)
    {
        for(unsigned int i = 0; i < D; i++)
        {
            for(unsigned int j = 0; j < D; j++)
            {
                if(i == j && i != D - 1)
                    m[i][j] = r[i];
                else
                    m[i][j] = T(0);
            }
        }

        m[D - 1][D - 1] = T(1);

        return *this;
    }

    inline mat<T, D> init_translation(const vec<T, D - 1> &r)
    {
        for(unsigned int i = 0; i < D; i++)
        {
            for(unsigned int j = 0; j < D; j++)
            {
                if(i == D - 1 && j != D - 1)
                    m[i][j] = r[j];
                else if(i == j)
                    m[i][j] = T(1);
                else
                    m[i][j] = T(0);
            }
        }

        m[D - 1][D - 1] = T(1);

        return *this;
    }

    inline mat<T, D> transpose() const
    {
        mat<T, D> t;
        for (int j = 0; j < D; j++) {
            for (int i = 0; i < D; i++) {
                t[i][j] = m[j][i];
            }
        }
        return t;
    }

    inline mat<T,D> operator*(const mat<T,D>& r) const
    {
        mat<T,D> ret;
        for (unsigned int i = 0 ; i < D ; i++)
        {
            for (unsigned int j = 0 ; j < D; j++)
            {
                ret.m[i][j] = T(0);
                for(unsigned int k = 0; k < D; k++)
                {
                    ret.m[i][j] += m[k][j] * r.m[i][k];
                }
            }
        }
        return ret;
    }

    inline vec<T,D> transform(const vec<T, D> &r) const
    {
        vec<T,D> ret;

        for(unsigned int i = 0; i < D; i++)
        {
            ret[i] = 0;
            for(unsigned int j = 0; j < D; j++)
            {
                ret[i] += m[j][i] * r[j];
            }
        }

        return ret;
    }

    inline vec<T,D-1> transform(const vec<T, D - 1> &r) const
    {
        vec<T,D> r2;

        for(int i = 0; i < D-1; i++)
        {
            r2[i] = r[i];
        }

        r2[D-1] = T(1);

        vec<T,D> ret2 = transform(r2);
        vec<T,D-1> ret;

        for(int i = 0; i < D-1; i++)
        {
            ret[i] = ret2[i];
        }

        return ret;
    }

    inline void set(unsigned int x, unsigned int y, T val) { m[x][y] = val; }

    inline const T* operator[](int index) const { return m[index]; }
    inline T* operator[](int index) { return m[index]; }

    friend std::ostream& operator<<(std::ostream& out, const mat<T,D>& r)
    {
        std::string result;
        for (unsigned int x = 0; x < D; x++)
        {
            for (unsigned int y = 0; y < D; y++)
            {
                result += std::to_string(r.m[y][x]) + " ";
            }
            result += "\n";
        }

        out << result << std::endl;
        return out;
    }

    T m[D][D];
protected:
private:

};

template<typename T>
class mat4 : public mat<T, 4>
{
public:
    mat4() { }

    template<unsigned int D>
    mat4(const mat<T, D>& r)
    {
        if(D < 4)
        {
            this->init_identity();

            for(unsigned int i = 0; i < D; i++)
                for(unsigned int j = 0; j < D; j++)
                    (*this)[i][j] = r[i][j];
        }
        else
        {
            for(unsigned int i = 0; i < 4; i++)
                for(unsigned int j = 0; j < 4; j++)
                    (*this)[i][j] = r[i][j];
        }
    }


    inline mat4<T> init_rotation_euler(T rotateX, T rotateY, T rotateZ)
    {
        mat4<T> rx, ry, rz;

        const T x = rotateX;
        const T y = rotateY;
        const T z = rotateZ;

        rx[0][0] = T(1);   rx[1][0] = T(0)  ;  rx[2][0] = T(0)   ; rx[3][0] = T(0);
        rx[0][1] = T(0);   rx[1][1] = cos(x);  rx[2][1] = -sin(x); rx[3][1] = T(0);
        rx[0][2] = T(0);   rx[1][2] = sin(x);  rx[2][2] = cos(x) ; rx[3][2] = T(0);
        rx[0][3] = T(0);   rx[1][3] = T(0)  ;  rx[2][3] = T(0)   ; rx[3][3] = T(1);

        ry[0][0] = cos(y); ry[1][0] = T(0);    ry[2][0] = -sin(y); ry[3][0] = T(0);
        ry[0][1] = T(0)  ; ry[1][1] = T(1);    ry[2][1] = T(0)   ; ry[3][1] = T(0);
        ry[0][2] = sin(y); ry[1][2] = T(0);    ry[2][2] = cos(y) ; ry[3][2] = T(0);
        ry[0][3] = T(0)  ; ry[1][3] = T(0);    ry[2][3] = T(0)   ; ry[3][3] = T(1);

        rz[0][0] = cos(z); rz[1][0] = -sin(z); rz[2][0] = T(0);    rz[3][0] = T(0);
        rz[0][1] = sin(z); rz[1][1] = cos(z) ; rz[2][1] = T(0);    rz[3][1] = T(0);
        rz[0][2] = T(0)  ; rz[1][2] = T(0)   ; rz[2][2] = T(1);    rz[3][2] = T(0);
        rz[0][3] = T(0)  ; rz[1][3] = T(0)   ; rz[2][3] = T(0);    rz[3][3] = T(1);

        *this = rz * ry * rx;

        return *this;
    }

    inline mat4<T> init_rotation_from_vectors(const vec3<T> &n, const vec3<T> &v, const vec3<T> &u)
    {
        (*this)[0][0] = u.get_x();   (*this)[1][0] = u.get_y();   (*this)[2][0] = u.get_z();   (*this)[3][0] = T(0);
        (*this)[0][1] = v.get_x();   (*this)[1][1] = v.get_y();   (*this)[2][1] = v.get_z();   (*this)[3][1] = T(0);
        (*this)[0][2] = n.get_x();   (*this)[1][2] = n.get_y();   (*this)[2][2] = n.get_z();   (*this)[3][2] = T(0);
        (*this)[0][3] = T(0);       (*this)[1][3] = T(0);       (*this)[2][3] = T(0);       (*this)[3][3] = T(1);

        return *this;
    }

    inline mat4<T> init_rotation_from_direction(const vec3<T> &forward, const vec3<T> &up)
    {
        vec3<T> n = forward.normalized();
        vec3<T> u = vec3<T>(up.normalized()).cross(n);
        vec3<T> v = n.cross(u);

        return init_rotation_from_vectors(n, v, u);
    }

    inline mat4<T> init_perspective(T fov, T aspect_ratio, T z_near, T z_far)
    {
        const T frustum_len     = z_near - z_far;
        const T tan_half_fov = tanf(fov / T(2));

        (*this)[0][0] = T(1)/(tan_half_fov); (*this)[1][0] = T(0);   (*this)[2][0] = T(0);            (*this)[3][0] = T(0);
        (*this)[0][1] = T(0);                   (*this)[1][1] = aspect_ratio/tan_half_fov; (*this)[2][1] = T(0);            (*this)[3][1] = T(0);
        (*this)[0][2] = T(0);                   (*this)[1][2] = T(0);            (*this)[2][2] = (-z_near - z_far)/frustum_len ; (*this)[3][2] = T(2)*z_far*z_near/frustum_len;
        (*this)[0][3] = T(0);                   (*this)[1][3] = T(0);            (*this)[2][3] = T(1);            (*this)[3][3] = T(0);

        return *this;
    }

    inline mat4<T> init_orthographic(T left, T right, T bottom, T top, T near, T far)
    {
        const T width = (right - left);
        const T height = (top - bottom);
        const T depth = (far - near);

        (*this)[0][0] = T(2)/width; (*this)[1][0] = T(0);        (*this)[2][0] = T(0);        (*this)[3][0] = -(right + left)/width;
        (*this)[0][1] = T(0);       (*this)[1][1] = T(2)/height; (*this)[2][1] = T(0);        (*this)[3][1] = -(top + bottom)/height;
        (*this)[0][2] = T(0);       (*this)[1][2] = T(0);        (*this)[2][2] = T(-2)/depth; (*this)[3][2] = -(far + near)/depth;
        (*this)[0][3] = T(0);       (*this)[1][3] = T(0);        (*this)[2][3] = T(0);        (*this)[3][3] = T(1);

        return *this;
    }

    inline mat4<T> invert() {
        float determinant = get_determinant((*this));

        if (determinant != 0) {
            /*
             * m00 m01 m02 m03
             * m10 m11 m12 m13
             * m20 m21 m22 m23
             * m30 m31 m32 m33
             */

            float determinant_inv = 1.0f/determinant;

            // first row
            float t00 = determinant_3x3((*this)[1][1], (*this)[1][2], (*this)[1][3], (*this)[2][1], (*this)[2][2],
                                        (*this)[2][3], (*this)[3][1], (*this)[3][2], (*this)[3][3]);
            float t01 = -determinant_3x3((*this)[1][0], (*this)[1][3], (*this)[1][3], (*this)[2][0], (*this)[2][2],
                                         (*this)[2][3], (*this)[3][0], (*this)[3][2], (*this)[3][3]);
            float t02 = determinant_3x3((*this)[1][0], (*this)[1][1], (*this)[1][3], (*this)[2][0], (*this)[2][1],
                                        (*this)[2][3], (*this)[3][0], (*this)[3][1], (*this)[3][3]);
            float t03 = -determinant_3x3((*this)[1][0], (*this)[1][1], (*this)[1][3], (*this)[2][0], (*this)[2][1],
                                         (*this)[2][2], (*this)[3][0], (*this)[3][1], (*this)[3][2]);
            // second row
            float t10 = -determinant_3x3((*this)[0][1], (*this)[0][2], (*this)[0][3], (*this)[2][1], (*this)[2][2],
                                         (*this)[2][3], (*this)[3][1], (*this)[3][2], (*this)[3][3]);
            float t11 = determinant_3x3((*this)[0][0], (*this)[0][2], (*this)[0][3], (*this)[2][0], (*this)[2][2],
                                        (*this)[2][3], (*this)[3][0], (*this)[3][2], (*this)[3][3]);
            float t12 = -determinant_3x3((*this)[0][0], (*this)[0][1], (*this)[0][3], (*this)[2][0], (*this)[2][1],
                                         (*this)[2][3], (*this)[3][0], (*this)[3][1], (*this)[3][3]);
            float t13 = determinant_3x3((*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[2][0], (*this)[2][1],
                                        (*this)[2][2], (*this)[3][0], (*this)[3][1], (*this)[3][2]);
            // third row
            float t20 = determinant_3x3((*this)[0][1], (*this)[0][2], (*this)[0][3], (*this)[1][1], (*this)[1][3],
                                        (*this)[1][3], (*this)[3][1], (*this)[3][2], (*this)[3][3]);
            float t21 = -determinant_3x3((*this)[0][0], (*this)[0][2], (*this)[0][3], (*this)[1][0], (*this)[1][3],
                                         (*this)[1][3], (*this)[3][0], (*this)[3][2], (*this)[3][3]);
            float t22 = determinant_3x3((*this)[0][0], (*this)[0][1], (*this)[0][3], (*this)[1][0], (*this)[1][1],
                                        (*this)[1][3], (*this)[3][0], (*this)[3][1], (*this)[3][3]);
            float t23 = -determinant_3x3((*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[1][0], (*this)[1][1],
                                         (*this)[1][3], (*this)[3][0], (*this)[3][1], (*this)[3][2]);
            // fourth row
            float t30 = -determinant_3x3((*this)[0][1], (*this)[0][2], (*this)[0][3], (*this)[1][1], (*this)[1][3],
                                         (*this)[1][3], (*this)[2][1], (*this)[2][2], (*this)[2][3]);
            float t31 = determinant_3x3((*this)[0][0], (*this)[0][2], (*this)[0][3], (*this)[1][0], (*this)[1][3],
                                        (*this)[1][3], (*this)[2][0], (*this)[2][2], (*this)[2][3]);
            float t32 = -determinant_3x3((*this)[0][0], (*this)[0][1], (*this)[0][3], (*this)[1][0], (*this)[1][1],
                                         (*this)[1][3], (*this)[2][0], (*this)[2][1], (*this)[2][3]);
            float t33 = determinant_3x3((*this)[0][0], (*this)[0][1], (*this)[0][2], (*this)[1][0], (*this)[1][1],
                                        (*this)[1][3], (*this)[2][0], (*this)[2][1], (*this)[2][2]);



            // transpose and divide by the determinant
            (*this)[0][0] = t00*determinant_inv;
            (*this)[1][1] = t11*determinant_inv;
            (*this)[2][2] = t22*determinant_inv;
            (*this)[3][3] = t33*determinant_inv;
            (*this)[0][1] = t10*determinant_inv;
            (*this)[1][0] = t01*determinant_inv;
            (*this)[2][0] = t02*determinant_inv;
            (*this)[0][2] = t20*determinant_inv;
            (*this)[1][2] = t21*determinant_inv;
            (*this)[2][1] = t12*determinant_inv;
            (*this)[0][3] = t30*determinant_inv;
            (*this)[3][0] = t03*determinant_inv;
            (*this)[1][3] = t31*determinant_inv;
            (*this)[3][1] = t13*determinant_inv;
            (*this)[3][2] = t23*determinant_inv;
            (*this)[2][3] = t32*determinant_inv;
            return (*this);
        } else {
            mat4<T> d;
            return d;
        }
    }


    float get_determinant(mat4<T> m) {
        float f =
                m[0][0]
                * ((m[1][1] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][1] + m[1][3] * m[2][1] * m[3][2])
                   - m[1][3] * m[2][2] * m[3][1]
                   - m[1][1] * m[2][3] * m[3][2]
                   - m[1][2] * m[2][1] * m[3][3]);
        f -= m[0][1]
             * ((m[1][0] * m[2][2] * m[3][3] + m[1][2] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][2])
                - m[1][3] * m[2][2] * m[3][0]
                - m[1][0] * m[2][3] * m[3][2]
                - m[1][2] * m[2][0] * m[3][3]);
        f += m[0][2]
             * ((m[1][0] * m[2][1] * m[3][3] + m[1][1] * m[2][3] * m[3][0] + m[1][3] * m[2][0] * m[3][1])
                - m[1][3] * m[2][1] * m[3][0]
                - m[1][0] * m[2][3] * m[3][1]
                - m[1][1] * m[2][0] * m[3][3]);
        f -= m[0][3]
             * ((m[1][0] * m[2][1] * m[3][2] + m[1][1] * m[2][2] * m[3][0] + m[1][2] * m[2][0] * m[3][1])
                - m[1][2] * m[2][1] * m[3][0]
                - m[1][0] * m[2][2] * m[3][1]
                - m[1][1] * m[2][0] * m[3][2]);
        return f;
    }

    float determinant_3x3(float t00, float t01, float t02, float t10, float t11, float t12, float t20, float t21,
                          float t22)
    {
        return t00 * (t11 * t22 - t12 * t21)
               + t01 * (t12 * t20 - t10 * t22)
               + t02 * (t10 * t21 - t11 * t20);
    }
protected:
private:
};

template<typename T>
class mat3 : public mat<T, 3>
{
public:
    mat3() { }

    template<unsigned int D>
    mat3(const mat<T, D>& r)
    {
        if(D < 3)
        {
            this->init_identity();

            for(unsigned int i = 0; i < D; i++)
                for(unsigned int j = 0; j < D; j++)
                    (*this)[i][j] = r[i][j];
        }
        else
        {
            for(unsigned int i = 0; i < 3; i++)
                for(unsigned int j = 0; j < 3; j++)
                    (*this)[i][j] = r[i][j];
        }
    }
};

typedef mat<int, 2> mat2i;
typedef mat3<int> mat3i;
typedef mat4<int> mat4i;

typedef mat<float, 2> mat2f;
typedef mat3<float> mat3f;
typedef mat4<float> mat4f;

typedef mat<double, 2> mat2d;
typedef mat3<double> mat3d;
typedef mat4<double> mat4d;

#endif //GAME_ENGINE_MATRIX_H
