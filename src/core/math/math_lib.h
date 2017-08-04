#ifndef ___D_rendering_engine__math3d__
#define ___D_rendering_engine__math3d__

#include <math.h>
#include <iostream>
#define MATH_PI 3.1415926535897932384626433832795
#define ToRadians(x) (float)(((x) * MATH_PI / 180.0f))
#define ToDegrees(x) (float)(((x) * 180.0f / MATH_PI))

template<typename T>
inline T Clamp(const T &a, const T &min, const T &max)
{
	if (a < min) return min;
	else if (a > max) return max; 
	else return a;
}

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
	
	inline T LengthSq() const { return this->dot(*this); }
	inline T Length() const { return sqrt(LengthSq()); }
	inline vec<T,D> normalized() const { return *this/Length(); }
	inline vec<T,D> lerp(const vec<T, D> &r, T lerpFactor) const { return (r - *this) * lerpFactor + *this; }

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
        
        out << result << std::endl;
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
	
	inline vec2<T> get_xy() const { return vec2<T>(get_x(), get_y()); }
	inline vec2<T> get_yz() const { return vec2<T>(get_y(), get_z()); }
	inline vec2<T> get_zx() const { return vec2<T>(get_z(), get_x()); }
	
	inline vec2<T> get_yx() const { return vec2<T>(get_y(), get_x()); }
	inline vec2<T> get_zy() const { return vec2<T>(get_z(), get_y()); }
	inline vec2<T> get_xz() const { return vec2<T>(get_x(), get_z()); }
	
	inline T get_x() const { return (*this)[0]; }
	inline T get_y() const { return (*this)[1]; }
	inline T get_z() const { return (*this)[2]; }
	
	inline void set_x(const T &x) { (*this)[0] = x; }
	inline void set_y(const T &y) { (*this)[1] = y; }
	inline void set_z(const T &z) { (*this)[2] = z; }

    inline void set(const T &x, const T &y, const T &z) { set_x(x);
        set_y(y);
        set_z(z); }
    inline void set(const vec3<T> &v) {
        set_x(v.get_x());
        set_y(v.get_y());
        set_z(v.get_z()); }
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
	
	inline void Set(const T& x, const T& y, const T& z, const T& w) { set_x(x);
		set_y(y);
		set_z(z);
		set_w(w); }
protected:
private:
};

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

	inline mat<T, D> inverse() const
	{
		int i, j, k;
		mat<T, D> s;
		s.init_identity();
		mat<T, D> t(*this);

		// Forward elimination
		for (i = 0; i < D - 1 ; i++) {
			int pivot = i;

			T pivotsize = t[i][i];

			if (pivotsize < 0)
				pivotsize = -pivotsize;

			for (j = i + 1; j < D; j++) {
				T tmp = t[j][i];

				if (tmp < 0)
					tmp = -tmp;

				if (tmp > pivotsize) {
					pivot = j;
					pivotsize = tmp;
				}
			}

			if (pivotsize == 0) {
				//if (singExc)
				//	throw ::Imath::SingMatrixExc ("Cannot invert singular matrix.");

				return mat<T, D>();
			}

			if (pivot != i) {
				for (j = 0; j < D; j++) {
					T tmp;

					tmp = t[i][j];
					t[i][j] = t[pivot][j];
					t[pivot][j] = tmp;

					tmp = s[i][j];
					s[i][j] = s[pivot][j];
					s[pivot][j] = tmp;
				}
			}

			for (j = i + 1; j < D; j++) {
				T f = t[j][i] / t[i][i];

				for (k = 0; k < D; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		// Backward substitution
		for (i = D - 1; i >= 0; --i) {
			T f;

			if ((f = t[i][i]) == 0) {
				//if (singExc)
				//	throw ::Imath::SingMatrixExc ("Cannot invert singular matrix.");

				return mat<T, D>();
			}

			for (j = 0; j < D; j++) {
				t[i][j] /= f;
				s[i][j] /= f;
			}

			for (j = 0; j < i; j++) {
				f = t[j][i];

				for (k = 0; k < D; k++) {
					t[j][k] -= f * t[i][k];
					s[j][k] -= f * s[i][k];
				}
			}
		}

		return s;
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
					ret.m[i][j] += m[k][j] * r.m[i][k];
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
				ret[i] += m[j][i] * r[j];
		}
		
		return ret;
	}
	
	inline vec<T,D-1> transform(const vec<T, D - 1> &r) const
	{
		vec<T,D> r2;
		
		for(int i = 0; i < D-1; i++)
			r2[i] = r[i];
			
		r2[D-1] = T(1);
		
		vec<T,D> ret2 = transform(r2);
		vec<T,D-1> ret;
		
		for(int i = 0; i < D-1; i++)
			ret[i] = ret2[i];
			
		return ret;
	}
	
	inline void Set(unsigned int x, unsigned int y, T val) { m[x][y] = val; }
	
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
	
	inline mat4<T> init_perspective(T fov, T aspectRatio, T zNear, T zFar)
	{
		const T zRange     = zNear - zFar;
		const T tanHalfFOV = tanf(fov / T(2));

		(*this)[0][0] = T(1)/(tanHalfFOV * aspectRatio); (*this)[1][0] = T(0);   (*this)[2][0] = T(0);            (*this)[3][0] = T(0);
		(*this)[0][1] = T(0);                   (*this)[1][1] = T(1)/tanHalfFOV; (*this)[2][1] = T(0);            (*this)[3][1] = T(0);
		(*this)[0][2] = T(0);                   (*this)[1][2] = T(0);            (*this)[2][2] = (-zNear - zFar)/zRange ; (*this)[3][2] = T(2)*zFar*zNear/zRange;
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

class quaternion;

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

	inline float Length() const { return sqrtf(get_x() * get_x() + get_y() * get_y() + get_z() * get_z()); }
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

	vec3f rotate(const quaternion &rotation) const;

	inline vec3f normalized() const
	{
		const float length = Length();

		return vec3f(get_x() / length, get_y() / length, get_z() / length);
	}

	inline vec3f operator+(const vec3f& r) const { return vec3f(get_x() + r.get_x(), get_y() + r.get_y(), get_z() +
                r.get_z()); }
	inline vec3f operator-(const vec3f& r) const { return vec3f(get_x() - r.get_x(), get_y() - r.get_y(), get_z() -
                r.get_z()); }
	inline vec3f operator*(float f) const { return vec3f(get_x() * f, get_y() * f, get_z() * f); }
	inline vec3f operator/(float f) const { return vec3f(get_x() / f, get_y() / f, get_z() / f); }

	inline bool operator==(const vec3f& r) const { return get_x() == r.get_x() && get_y() == r.get_y() && get_z() ==
                                                                                                                  r.get_z(); }
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
        return vec3f((*this)[1] * r.get_z() - (*this)[2] * r.get_y(), (*this)[2] * r.get_x() - (*this)[0] * r.get_z(), (*this)[0] *
                                                                                                                            r.get_y() - (*this)[1] *
                                                                                                                                                   r.get_x());
    }

	inline float get_x() const { return (*this)[0]; }
	inline float get_y() const { return (*this)[1]; }
	inline float get_z() const { return (*this)[2]; }

	inline void set_x(float x) { (*this)[0] = x; }
	inline void set_y(float y) { (*this)[1] = y; }
	inline void set_z(float z) { (*this)[2] = z; }

	inline void Set(float x, float y, float z) { (*this)[0] = x; (*this)[1] = y; (*this)[2] = z; }
private:
	//float x,y,z;
};

typedef vec2<int> vec2i;
typedef vec3<int> vec3i;
typedef vec4<int> vec4i;

typedef vec2<float> vec2f;
//typedef vector3<float> vec3f;
typedef vec4<float> vec4f;

typedef vec2<double> vec2d;
typedef vec3<double> vec3d;
typedef vec4<double> vec4d;

typedef mat<int, 2> mat2i;
typedef mat3<int> mat3i;
typedef mat4<int> mat4i;

typedef mat<float, 2> mat2f;
typedef mat3<float> mat3f;
typedef mat4<float> mat4f;

typedef mat<double, 2> mat2d;
typedef mat3<double> mat3d;
typedef mat4<double> mat4d;

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
		
		float length = Length();
		(*this)[3] = (*this)[3] / length;
		(*this)[0] = (*this)[0] / length;
		(*this)[1] = (*this)[1] / length;
		(*this)[2] = (*this)[2] / length;
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
		
		float sin = (float)sqrtf(1.0f - cos * cos);
		float angle = atan2(sin, cos);
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

	inline quaternion conjugate() const { return quaternion(-get_x(), -get_y(), -get_z(), get_w()); }

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

#endif
