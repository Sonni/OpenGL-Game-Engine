#ifndef GAME_ENGINE_RENDERING_ENGINE_H
#define GAME_ENGINE_RENDERING_ENGINE_H

#include "math/math.h"

class transform
{
public:
	transform(const vec3f& pos = vec3f(0,0,0), const quaternion& rot = quaternion(0,0,0,1), float scale = 1.0f) :
			m_pos(pos),
		m_rot(rot),
		m_scale(scale),
		m_use_trans_mat(false)
	{}

	mat4f get_transformation() const;
	void rotate(const vec3f &axis, float angle);
	void rotate(const quaternion &rotation);
	void look_at(const vec3f& point, const vec3f& up);
	
	inline quaternion get_look_at_rotation(const vec3f &point, const vec3f &up)
	{ 
		return quaternion(mat4f().init_rotation_from_direction((point - m_pos).normalized(), up));
	}
	
	inline vec3f* get_pos() { return &m_pos; }
	inline const vec3f& get_pos() const { return m_pos; }
	inline quaternion* get_rot() { return &m_rot; }
	inline const quaternion& get_rot() const { return m_rot; }
	inline float get_scale() const { return m_scale; }

	inline void set_pos(const vec3f &pos) { m_pos = pos; }
	inline void set_rot(const quaternion &rot) { m_rot = rot; }
	inline void set_scale(float scale) { m_scale = scale; }

    inline void set_transformation_matrix(mat4f mat) { m_use_trans_mat = true; m_transformation_matrix = mat;}
private:
	vec3f m_pos;
	quaternion m_rot;
	float m_scale;
    mat4f m_transformation_matrix;
    bool m_use_trans_mat;
};

#endif