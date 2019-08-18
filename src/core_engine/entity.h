#ifndef GAME_ENGINE_ENTITY_H
#define GAME_ENGINE_ENTITY_H

#include <vector>
#include "transform.h"
#include "io/input.h"
#include "../physics_engine/physics_obj.h"

class shader;
class entity_component;
class camera;

class entity
{
public:
    entity(shader *shader = nullptr, const vec3f& pos = vec3f(0, 0, 0), const quaternion& rot = quaternion(0, 0, 0, 1), float scale = 1.0f) :
            m_transform(pos, rot, scale), m_shader_program(shader)
    { }

    virtual ~entity();

    entity* add_component(entity_component *component);

    void set_all_uni(camera& cam);
    void process_input(const input &input, float delta);
    void update(float delta, const camera &cam);
    void render() const;

    void delete_component(entity_component *component);

    inline shader* get_shader() {return m_shader_program;}
    inline transform* get_transform() { return &m_transform; }
protected:
private:
    std::vector<entity_component*> m_components;
    transform m_transform;
    shader* m_shader_program;


};
#endif