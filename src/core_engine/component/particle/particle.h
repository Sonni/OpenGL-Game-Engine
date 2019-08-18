#ifndef INC_3D_ENGINE_PARTICLE_H
#define INC_3D_ENGINE_PARTICLE_H


#include "../../../rendering_engine/model/mesh.h"
#include "../../../shader/shader.h"
#include "../../../rendering_engine/texture.h"

class particle
{
public:

    particle(int num_of_rows, transform _transform, vec3f velocity, float gravityEffect, float lifeLength) :
            m_num_of_rows(num_of_rows), m_transform(_transform), m_velocity(velocity), m_gravity(gravityEffect), m_life_length(lifeLength)
    { }
    ~particle() { }


    void set_all_uni(camera& cam, std::vector<float>* data);
    void update(float delta, camera* cam);


    inline float get_distance() const { return m_distance; }
    inline bool get_is_dead() const { return m_is_dead; }

private:
    vec3f m_velocity;
    transform m_transform;
    float m_gravity;
    float m_life_length;
    float m_elapsed_time = 0;
    bool m_is_dead = false;

    float m_distance = 0;

    vec2f m_tex_offset1, m_tex_offset2;
    float m_blend;
    int m_num_of_rows;

    void set_tex_offset(vec2f* offset, int index);
    void update_tex_coord_info();
};

#endif //INC_3D_ENGINE_PARTICLE_H
