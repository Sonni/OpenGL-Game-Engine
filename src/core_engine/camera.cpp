#include "camera.h"

mat4f camera::get_view_projection() const
{
    //Get the conjugate of the rot matrix to simulate the the world is rotating the other direction
    mat4f cam_conj_rot_matrix = get_transform().get_rot().negate().to_rotation_matrix();
    mat4f cam_translation_matrix;
    
    //*-1 to invert, because the world should appear to move
    cam_translation_matrix.init_translation(get_transform().get_pos() * -1);
    
    return projection_matrix * cam_conj_rot_matrix * cam_translation_matrix;
}

mat4f camera::get_view_matrix() const
{
    mat4f cam_rot_matrix;
    cam_rot_matrix.init_rotation_from_direction(transformation->get_rot()->get_forward(), transformation->get_rot()->get_up());

    mat4f cam_translation_matrix;
    cam_translation_matrix.init_translation(*transformation->get_pos());
    
    return cam_rot_matrix * cam_translation_matrix;
}

quaternion camera::invert_pitch(quaternion old_rot)
{
    float new_pitch = -ToDegrees(asinf(transformation->get_rot()->get_forward().get_y()));
    float cur_pitch = ToDegrees(asinf(transformation->get_rot()->get_forward().get_y()));
    float amount = new_pitch - cur_pitch;


    quaternion tmp_pitch = quaternion(vec3f(1, 0, 0), ToRadians(-amount));

    transformation->set_rot(old_rot * tmp_pitch);
    return old_rot;
}
