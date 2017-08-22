#include "particle.h"

void particle::set_all_uni(camera& cam, std::vector<float>* data)
{
    mat4f modelMatrix;
    modelMatrix.init_identity();
    mat4f translationMatrix;
    translationMatrix.init_translation(*_transform.get_pos());
    modelMatrix = modelMatrix * translationMatrix;


    modelMatrix.m[0][0] = cam.get_view_matrix().m[0][0];
    modelMatrix.m[0][1] = cam.get_view_matrix().m[1][0];
    modelMatrix.m[0][2] = cam.get_view_matrix().m[2][0];
    modelMatrix.m[1][0] = cam.get_view_matrix().m[0][1];
    modelMatrix.m[1][1] = cam.get_view_matrix().m[1][1];
    modelMatrix.m[1][2] = cam.get_view_matrix().m[2][1];
    modelMatrix.m[2][0] = cam.get_view_matrix().m[0][2];
    modelMatrix.m[2][1] = cam.get_view_matrix().m[1][2];
    modelMatrix.m[2][2] = cam.get_view_matrix().m[2][2];

    mat4f rotationMatrix;
    rotationMatrix.init_rotation_euler(_transform.get_rot()->get_x(), _transform.get_rot()->get_y(), _transform.get_rot()->get_z());
    modelMatrix = modelMatrix * rotationMatrix;

    mat4f scaleMatrix;
    scaleMatrix.init_scale(vec3f(_transform.get_scale(), _transform.get_scale(), _transform.get_scale()));
    modelMatrix = modelMatrix * scaleMatrix;


    data->push_back(modelMatrix[0][0]); data->push_back(modelMatrix[0][1]); data->push_back(modelMatrix[0][2]); data->push_back(modelMatrix[0][3]);
    data->push_back(modelMatrix[1][0]); data->push_back(modelMatrix[1][1]); data->push_back(modelMatrix[1][2]); data->push_back(modelMatrix[1][3]);
    data->push_back(modelMatrix[2][0]); data->push_back(modelMatrix[2][1]); data->push_back(modelMatrix[2][2]); data->push_back(modelMatrix[2][3]);
    data->push_back(modelMatrix[3][0]); data->push_back(modelMatrix[3][1]); data->push_back(modelMatrix[3][2]); data->push_back(modelMatrix[3][3]);

    data->push_back(tex_offset1.get_x()); data->push_back(tex_offset1.get_y());
    data->push_back(tex_offset2.get_x()); data->push_back(tex_offset2.get_y());
    data->push_back(blend);
}

void particle::update(float delta, camera* cam)
{
    elapsed_time += delta;
    if (elapsed_time > life_length)
    {
        is_dead = true;
        return;
    }

    velocity.set_y(velocity.get_y() + gravity * delta);
    _transform.set_pos((velocity * 0.008f) + *_transform.get_pos());
    distance = (*cam->get_transform()->get_pos() - *_transform.get_pos()).length_sq();

    update_tex_coord_info();
}

void particle::set_tex_offset(vec2f* offset, int index)
{
    int column = index % num_of_rows;
    int row = index / num_of_rows;
    offset->set_x((float) column / num_of_rows);
    offset->set_y((float) row / num_of_rows);
}

void particle::update_tex_coord_info()
{
    float life_factor = elapsed_time / life_length;
    int stage_count = num_of_rows * num_of_rows;
    float atlas_progress = life_factor * stage_count;
    int index1 = (int) floor(atlas_progress);
    int index2 = index1 < stage_count - 1 ? index1 + 1 : index1;
    blend = (float) fmod(atlas_progress, 1.0f);
    set_tex_offset(&tex_offset1, index1);
    set_tex_offset(&tex_offset2, index2);
}