#ifndef INC_3D_ENGINE_PARTICLE_SYSTEM_HPP
#define INC_3D_ENGINE_PARTICLE_SYSTEM_HPP


#include "../../entity_component.hpp"
#include "../../../rendering_engine/model/mesh.hpp"
#include "particle.hpp"
#include "../../../rendering_engine/texture.hpp"

class particle_system : public entity_component
{
public:
    particle_system(std::string tex_name, int num_of_rows, camera* cam, float pps, float speed, float gravityComplient, float lifeLength, float average_scale);

    virtual ~particle_system() {}

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void update(float delta, const camera &cam);

    inline void set_speed_error(float error) { m_speed_offset = error * m_average_speed; }
    inline void set_life_error(float error) { m_life_offset = error * m_average_life_len; }
    inline void set_scale_error(float error) { m_scale_offset = error * m_average_scale; }
    inline void set_direction(vec3f direction, float deviation)
    {
        m_direction = vec3f(direction);
        m_direction_deviation = (float) (deviation * MATH_PI);
    }

private:
    const int m_MAX_INSTANCES = 10000;
    const int m_INSTANCED_DATA_LENGTH = 21;

    float m_pps, m_average_speed, m_gravity, m_average_life_len, m_average_scale;

    float m_speed_offset, m_life_offset, m_scale_offset;
    float m_direction_deviation;

    bool m_draw = true;

    vec3f m_direction;

    texture* m_tex;

    std::vector<particle> m_particles;
    camera* m_cam;

    GLuint m_vao_id;
    GLuint m_vbo;

    GLint m_view_projection_loc;
    GLint m_tex_loc;
    GLint m_num_of_rows_loc;

    int m_vertex_count;
    int m_num_of_rows;

    GLuint create_empty_vbo(int float_count) const;
    void update_vbo(GLuint vbo, std::vector<float> data) const;
    void add_instanced_attribute(GLuint vao, GLuint vbo, GLuint attribute, int data_size, int instanced_data_length, int offset) const;
    void set_attribute(GLuint attrib_num, int coord_size, std::vector<float> data) const;

    void generate_particles(const vec3f& system_pos, float delta);
    void emit_particle(vec3f center);
    float generate_value(float average, float error_margin) const;
    vec3f generate_rand_unit_vec_within_cone(vec3f cone_direction, float angle);
    vec3f generate_rand_unit_vec();

    void insertion_sort(std::vector<particle> *particle_list);
    void insertion_sort(std::vector<particle> *particle_list, int i);

    inline float get_rand_float() const { return (rand() / (float)RAND_MAX); }
};

#endif //INC_3D_ENGINE_PARTICLE_SYSTEM_HPP
