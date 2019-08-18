#include "particle.h"

void particle::set_all_uni(camera& cam, std::vector<float>* data)
{
    mat4f modelMatrix;
    modelMatrix.init_identity();
    mat4f translationMatrix;
    translationMatrix.init_translation(*m_transform.get_pos());
    modelMatrix = modelMatrix * translationMatrix;


    modelMatrix[0][0] = cam.get_view_matrix()[0][0];
    modelMatrix[0][1] = cam.get_view_matrix()[1][0];
    modelMatrix[0][2] = cam.get_view_matrix()[2][0];
    modelMatrix[1][0] = cam.get_view_matrix()[0][1];
    modelMatrix[1][1] = cam.get_view_matrix()[1][1];
    modelMatrix[1][2] = cam.get_view_matrix()[2][1];
    modelMatrix[2][0] = cam.get_view_matrix()[0][2];
    modelMatrix[2][1] = cam.get_view_matrix()[1][2];
    modelMatrix[2][2] = cam.get_view_matrix()[2][2];

    mat4f rotationMatrix;
    rotationMatrix.init_rotation_euler(m_transform.get_rot()->get_x(), m_transform.get_rot()->get_y(), m_transform.get_rot()->get_z());
    modelMatrix = modelMatrix * rotationMatrix;

    mat4f scaleMatrix;
    scaleMatrix.init_scale(vec3f(m_transform.get_scale(), m_transform.get_scale(), m_transform.get_scale()));
    modelMatrix = modelMatrix * scaleMatrix;


    data->push_back(modelMatrix[0][0]); data->push_back(modelMatrix[0][1]); data->push_back(modelMatrix[0][2]); data->push_back(modelMatrix[0][3]);
    data->push_back(modelMatrix[1][0]); data->push_back(modelMatrix[1][1]); data->push_back(modelMatrix[1][2]); data->push_back(modelMatrix[1][3]);
    data->push_back(modelMatrix[2][0]); data->push_back(modelMatrix[2][1]); data->push_back(modelMatrix[2][2]); data->push_back(modelMatrix[2][3]);
    data->push_back(modelMatrix[3][0]); data->push_back(modelMatrix[3][1]); data->push_back(modelMatrix[3][2]); data->push_back(modelMatrix[3][3]);

    data->push_back(m_tex_offset1.get_x()); data->push_back(m_tex_offset1.get_y());
    data->push_back(m_tex_offset2.get_x()); data->push_back(m_tex_offset2.get_y());
    data->push_back(m_blend);
}

void particle::update(float delta, camera* cam)
{
    m_elapsed_time += delta;
    if (m_elapsed_time > m_life_length)
    {
        m_is_dead = true;
        return;
    }

    m_velocity.set_y(m_velocity.get_y() + m_gravity * delta);
    m_transform.set_pos((m_velocity * 0.008f) + *m_transform.get_pos());
    m_distance = (*cam->get_transform()->get_pos() - *m_transform.get_pos()).length_sq();

    update_tex_coord_info();
}

void particle::set_tex_offset(vec2f* offset, int index)
{
    int column = index % m_num_of_rows;
    int row = index / m_num_of_rows;
    offset->set_x((float) column / m_num_of_rows);
    offset->set_y((float) row / m_num_of_rows);
}

void particle::update_tex_coord_info()
{
    float life_factor = m_elapsed_time / m_life_length;
    int stage_count = m_num_of_rows * m_num_of_rows;
    float atlas_progress = life_factor * stage_count;
    int index1 = (int) floor(atlas_progress);
    int index2 = index1 < stage_count - 1 ? index1 + 1 : index1;
    m_blend = (float) fmod(atlas_progress, 1.0f);
    set_tex_offset(&m_tex_offset1, index1);
    set_tex_offset(&m_tex_offset2, index2);
}