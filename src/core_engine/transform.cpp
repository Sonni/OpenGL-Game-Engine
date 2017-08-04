#include "transform.h"

void transform::rotate(const vec3f &axis, float angle)
{
	rotate(quaternion(axis, angle));
}

void transform::rotate(const quaternion &rotation)
{
	rot = quaternion((rotation * rot).normalized());
}

void transform::look_at(const vec3f& point, const vec3f& up)
{
	rot = get_look_at_rotation(point, up);
}

mat4f transform::get_transformation() const
{
    if(use_trans_mat) return transformation_matrix;
	mat4f translation_matrix;
	mat4f scale_matrix;

	translation_matrix.init_translation(get_pos());
	scale_matrix.init_scale(vec3f(scale, scale, scale));

	mat4f model_matrix = translation_matrix * rot.to_rotation_matrix() * scale_matrix;

	return model_matrix;
}

