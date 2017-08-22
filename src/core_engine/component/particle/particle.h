#ifndef INC_3D_ENGINE_PARTICLE_H
#define INC_3D_ENGINE_PARTICLE_H


#include "../../../rendering_engine/model/mesh.h"
#include "../../../shader/shader.h"
#include "../../../rendering_engine/texture.h"

class particle
{
public:

    particle(int num_of_rows, transform _transform, vec3f velocity, float gravityEffect, float lifeLength) :
           num_of_rows(num_of_rows), _transform(_transform), velocity(velocity), gravity(gravityEffect), life_length(lifeLength)
    { }
    ~particle() { }


    void set_all_uni(camera& cam, std::vector<float>* data, GLint view_projection_loc, GLint num_of_rows_loc);
    void update(float delta, camera* cam);


    inline float get_distance() const { return distance; }
    inline bool get_is_dead() const { return is_dead; }

private:
    vec3f velocity;
    transform _transform;
    float gravity;
    float life_length;
    float elapsed_time = 0;
    bool is_dead = false;

    float distance = 0;

    vec2f tex_offset1, tex_offset2;
    float blend;
    int num_of_rows;

    void set_tex_offset(vec2f* offset, int index);
    void update_tex_coord_info();
};

#endif //INC_3D_ENGINE_PARTICLE_H
