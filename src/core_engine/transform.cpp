#include "transform.h"

void transform::rotate(const vec3f &axis, float angle)
{
	rotate(quaternion(axis, angle));
}

void transform::rotate(const quaternion &rotation)
{
	m_rot = quaternion((rotation * m_rot).normalized());
}

void transform::look_at(const vec3f& point, const vec3f& up)
{
	m_rot = get_look_at_rotation(point, up);
}

mat4f transform::get_transformation() const
{
    if(m_use_trans_mat) return m_transformation_matrix;
	mat4f translation_matrix;
	mat4f scale_matrix;

	translation_matrix.init_translation(get_pos());
	scale_matrix.init_scale(vec3f(m_scale, m_scale, m_scale));

	mat4f model_matrix = translation_matrix * m_rot.to_rotation_matrix() * scale_matrix;

	return model_matrix;
}

