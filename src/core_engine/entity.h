#ifndef GAME_ENGINE_ENTITY_H
#define GAME_ENGINE_ENTITY_H

#include <vector>
#include "transform.h"
#include "io/input.h"
#include "../rendering_engine/light/light.h"

class shader;
class entity_component;
class camera;

class entity
{
public:
    entity(shader *shader = NULL, const vec3f& pos = vec3f(0,0,0), const quaternion& rot = quaternion(0,0,0,1), float scale = 1.0f) :
            _transform(pos, rot, scale)
    {
        shader_program = shader;
    }

    virtual ~entity();

    entity* add_component(entity_component *component);

    void set_all_uni(camera& cam);
    void process_input(const input &input, float delta);
    void update(float delta);
    void render() const;

    void delete_component(entity_component *component);


    inline shader* get_shader() {return shader_program;}
    inline transform* get_transform() { return &_transform; }
protected:
private:
    std::vector<entity_component*> components;
    transform _transform;
    shader* shader_program;


};
#endif