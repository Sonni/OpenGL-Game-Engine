#ifndef INC_3D_ENGINE_PARTICLE_SYSTEM_H
#define INC_3D_ENGINE_PARTICLE_SYSTEM_H


#include "../../entity_component.h"
#include "../../../rendering_engine/model/mesh.h"
#include "particle.h"
#include "../../../rendering_engine/texture.h"

class particle_system : public entity_component
{
public:
    particle_system(std::string tex_name, int num_of_rows, camera* cam, float pps, float speed, float gravityComplient, float lifeLength, float average_scale);

    virtual ~particle_system() {}

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void update(float delta);

    inline void set_speed_error(float error) { speed_offset = error * average_speed; }
    inline void set_life_error(float error) { life_offset = error * average_life_len; }
    inline void set_scale_error(float error) { scale_offset = error * average_scale; }
    inline void set_direction(vec3f direction, float deviation)
    {
        this->direction = vec3f(direction);
        this->direction_deviation = (float) (deviation * MATH_PI);
    }

private:
    const int MAX_INSTANCES = 10000;
    const int INSTANCED_DATA_LENGTH = 21;

    float pps, average_speed, gravity, average_life_len, average_scale;

    float speed_offset, life_offset, scale_offset;
    float direction_deviation;

    vec3f direction;

    texture* tex;

    std::vector<particle> particles;
    camera* cam;

    GLuint vao_id;
    GLuint vbo;

    GLint view_projection_loc;
    GLint tex_loc;
    GLint num_of_rows_loc;

    int vertex_count;
    int num_of_rows;

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

#endif //INC_3D_ENGINE_PARTICLE_SYSTEM_H
