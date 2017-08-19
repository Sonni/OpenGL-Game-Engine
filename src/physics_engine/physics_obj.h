#ifndef GAME_ENGINE_PHYSICS_OBJ_H
#define GAME_ENGINE_PHYSICS_OBJ_H


#include "../core_engine/math/vector.h"
#include "collider/collider.h"
#include "../core_engine/transform.h"

class physics_obj
{
public:

    physics_obj(collider* _collider, transform* _transform) :
    _transform(_transform),
    _collider(_collider)
    {}

    physics_obj(const physics_obj& other);
    void operator=(physics_obj other);
    virtual ~physics_obj();

 
    void integrate(float delta);

    inline const vec3f& get_pos() const { return *_transform->get_pos(); }
    inline const vec3f& get_velocity() const { return velocity; }

  
    collider* get_collider()
    {
        _collider->set_pos(*_transform->get_pos());

        return _collider;
    }

    inline void set_velocity(const vec3f &velocity) { this->velocity = velocity; }
    inline transform* get_transform() { return _transform; }

private:
    vec3f velocity;
    collider* _collider;
    transform* _transform;
};


#endif //GAME_ENGINE_PHYSICS_OBJ_H
