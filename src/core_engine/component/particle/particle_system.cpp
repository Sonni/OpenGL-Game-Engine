#include "particle_system.h"

particle_system::particle_system(std::string tex_name, int num_of_rows, camera* cam, float pps, float speed, float gravityComplient, float lifeLength, float average_scale) :
        m_num_of_rows(num_of_rows), m_cam(cam)
{
    m_pps = pps;
    m_average_speed = speed;
    m_gravity = gravityComplient;
    m_average_life_len = lifeLength;
    m_average_scale = average_scale;

    m_speed_offset = m_life_offset = m_scale_offset = 0;
    m_direction_deviation = 0;

    set_direction(vec3f(0, 1, 0), 0.1f);
    set_life_error(0.1f);
    set_scale_error(0.4f);
    set_speed_error(0.8f);

    m_tex = new texture("particle/" + tex_name);


    std::vector<float> verticies;
    verticies.push_back(-0.5f); verticies.push_back(0.5f); verticies.push_back(-0.5f); verticies.push_back(-0.5f);
    verticies.push_back(0.5f); verticies.push_back(0.5f); verticies.push_back(0.5f); verticies.push_back(-0.5f);

    m_vertex_count = (int) (verticies.size() / 2);


    glGenVertexArrays(1, &m_vao_id);
    glBindVertexArray(m_vao_id);
    set_attribute(0, 2, verticies);
    glBindVertexArray(0);

    m_vbo = create_empty_vbo(m_INSTANCED_DATA_LENGTH * m_MAX_INSTANCES);
    add_instanced_attribute(m_vao_id, m_vbo, 1, 4, m_INSTANCED_DATA_LENGTH, 0);
    add_instanced_attribute(m_vao_id, m_vbo, 2, 4, m_INSTANCED_DATA_LENGTH, 4);
    add_instanced_attribute(m_vao_id, m_vbo, 3, 4, m_INSTANCED_DATA_LENGTH, 8);
    add_instanced_attribute(m_vao_id, m_vbo, 4, 4, m_INSTANCED_DATA_LENGTH, 12);
    add_instanced_attribute(m_vao_id, m_vbo, 5, 4, m_INSTANCED_DATA_LENGTH, 16);
    add_instanced_attribute(m_vao_id, m_vbo, 6, 1, m_INSTANCED_DATA_LENGTH, 20);
}

void particle_system::init()
{
    m_view_projection_loc = get_shader()->get_uni_location("view_projection");
    m_tex_loc = get_shader()->get_uni_location("model_tex");
    m_num_of_rows_loc = get_shader()->get_uni_location("num_rows");
}

void particle_system::set_all_uni(camera& cam)
{
    if (m_draw)
    {
        std::vector<float> data;

        insertion_sort(&m_particles);

        for (unsigned int i = 0; i < m_particles.size(); i++) {
            m_particles[i].set_all_uni(cam, &data);
        }

        update_vbo(m_vbo, data);

        glUniformMatrix4fv(m_view_projection_loc, 1, GL_FALSE, &cam.get_view_projection()[0][0]);
        glUniform1f(m_num_of_rows_loc, m_num_of_rows);

        m_tex->bind(0);
        glUniform1i(m_tex_loc, 0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);

        glBindVertexArray(m_vao_id);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);
        glEnableVertexAttribArray(4);
        glEnableVertexAttribArray(5);
        glEnableVertexAttribArray(6);

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, m_vertex_count, m_particles.size());

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
        glDisableVertexAttribArray(4);
        glDisableVertexAttribArray(5);
        glDisableVertexAttribArray(6);
        glBindVertexArray(0);

        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
    }
}


void particle_system::update(float delta, const camera &cam)
{
    if (cam.get_frustum().sphere_in_frustum(sphere(*get_transform()->get_pos(), 40)))
        m_draw = true;
    else
        m_draw = false;

    generate_particles(*get_transform()->get_pos(), delta);

    for (unsigned int i = 0; i < m_particles.size(); i++)
    {
        m_particles[i].update(delta, m_cam);
        if (m_particles[i].get_is_dead())
        {
            m_particles.erase(m_particles.begin()+i);
        }
    }
}

GLuint particle_system::create_empty_vbo(int float_count) const
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, float_count * sizeof(float), NULL, GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vboID;
}

void particle_system::update_vbo(GLuint vbo, std::vector<float> data) const
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void particle_system::add_instanced_attribute(GLuint vao, GLuint vbo, GLuint attribute, int data_size, int instanced_data_length, int offset) const
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindVertexArray(vao);
    glVertexAttribPointer(attribute, data_size, GL_FLOAT, GL_FALSE, instanced_data_length * 4, (void*) (offset * sizeof(float)));
    glVertexAttribDivisor(attribute, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void particle_system::set_attribute(GLuint attrib_num, int coord_size, std::vector<float> data) const
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attrib_num, coord_size, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void particle_system::generate_particles(const vec3f& system_pos, float delta)
{
    float num_particles = m_pps * delta;
    int count = (int) ceil(num_particles);

    for (int i = 0; i < count; i++)
    {
        emit_particle(system_pos);
    }
}

void particle_system::emit_particle(vec3f center)
{
    vec3f velocity;
    if(m_direction != vec3f())
    {
        velocity = generate_rand_unit_vec_within_cone(m_direction, m_direction_deviation);
    }
    else
    {
        velocity = generate_rand_unit_vec();
    }
    velocity = velocity.normalized();
    velocity.scale(generate_value(m_average_speed, m_speed_offset));
    float scale = generate_value(m_average_scale, m_scale_offset);
    float lifeLength = generate_value(m_average_life_len, m_life_offset);

    transform t(center, quaternion(), scale);
    particle p(m_num_of_rows, t, velocity, m_gravity, lifeLength);
    m_particles.push_back(p);
}

float particle_system::generate_value(float average, float error_margin) const
{
    float offset = (get_rand_float() - 0.5f) * 2.0f * error_margin;
    return average + offset;
}


vec3f particle_system::generate_rand_unit_vec_within_cone(vec3f cone_direction, float angle)
{
    float cos_angle = (float) cos(angle);

    float theta = (float) (get_rand_float() * 2.0f * MATH_PI);
    float z = cos_angle + (get_rand_float() * (1 - cos_angle));
    float root_one_minus_z_squared = (float) sqrt(1 - z * z);
    float x = (float) (root_one_minus_z_squared * cos(theta));
    float y = (float) (root_one_minus_z_squared * sin(theta));

    vec4f direction(x, y, z, 1);
    if (cone_direction.get_x() != 0 || cone_direction.get_y() != 0 || (cone_direction.get_z() != 1 && cone_direction.get_z() != -1))
    {
        vec3f rot_axis = cone_direction.cross(vec3f(0, 0, 1));
        rot_axis = rot_axis.normalized();
        float rot_angle = (float) acos(cone_direction.dot(vec3f(0, 0, 1)));
        quaternion q(rot_axis, -rot_angle);
        mat4f rot_mat = q.to_rotation_matrix();

        direction = rot_mat.transform(direction);

    }
    else if (cone_direction.get_z() == -1)
    {
        direction.set_z(direction.get_z()*-1);
    }
    return vec3f(direction.get_x(), direction.get_y(), direction.get_z());
}

vec3f particle_system::generate_rand_unit_vec()
{
    float theta = (float) (get_rand_float() * 2.0f * MATH_PI);
    float z = (get_rand_float() * 2) - 1;
    float root_one_minus_z_squared = (float) sqrt(1 - z * z);
    float x = (float) (root_one_minus_z_squared * cos(theta));
    float y = (float) (root_one_minus_z_squared * sin(theta));
    return vec3f(x, y, z);
}

void particle_system::insertion_sort(std::vector<particle> *particle_list)
{
    for (int i = 1; i < particle_list->size(); i++)
    {
        particle item = particle_list->at(i);
        if (item.get_distance() > particle_list->at(i - 1).get_distance())
        {
            insertion_sort(particle_list, i);
        }
    }
}

void particle_system::insertion_sort(std::vector<particle> *particle_list, int i)
{
    particle item = particle_list->at(i);
    int pointer = i - 1;
    while (pointer != 0 && particle_list->at(pointer - 1).get_distance() < item.get_distance())
    {
        pointer--;
    }
    particle_list->erase(particle_list->begin() + i);
    particle_list->emplace(particle_list->begin() + pointer, item);
}