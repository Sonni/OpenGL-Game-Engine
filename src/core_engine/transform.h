#ifndef GAME_ENGINE_RENDERING_ENGINE_H
#define GAME_ENGINE_RENDERING_ENGINE_H


#include "math/math.h"
class transform
{
public:
	transform(const vec3f& pos = vec3f(0,0,0), const quaternion& rot = quaternion(0,0,0,1), float scale = 1.0f) :
		pos(pos),
		rot(rot),
		scale(scale),
        use_trans_mat(false)
	{}

	mat4f get_transformation() const;
	void rotate(const vec3f &axis, float angle);
	void rotate(const quaternion &rotation);
	void look_at(const vec3f& point, const vec3f& up);
	
	inline quaternion get_look_at_rotation(const vec3f &point, const vec3f &up)
	{ 
		return quaternion(mat4f().init_rotation_from_direction((point - pos).normalized(), up));
	}
	
	inline vec3f* get_pos() { return &pos; }
	inline const vec3f& get_pos() const { return pos; }
	inline quaternion* get_rot() { return &rot; }
	inline const quaternion& get_rot() const { return rot; }
	inline float get_scale() const { return scale; }

	inline void set_pos(const vec3f &pos) { this->pos = pos; }
	inline void set_rot(const quaternion &rot) { this->rot = rot; }
	inline void set_scale(float scale) { this->scale = scale; }

    inline void set_transformation_matrix(mat4f mat) { use_trans_mat = true; transformation_matrix = mat;}
private:
	vec3f pos;
	quaternion rot;
	float scale;
    mat4f transformation_matrix;
    bool use_trans_mat;
};

#endif