#ifndef GAME_ENGINE_PHYSICS_OBJ_H
#define GAME_ENGINE_PHYSICS_OBJ_H


#include "../core_engine/math/vector.hpp"
#include "collider/collider.hpp"
#include "../core_engine/transform.hpp"

class physics_obj
{
public:

    physics_obj(collider* _collider) :
    _collider(_collider)
    {}

    physics_obj(const physics_obj& other);
    void operator=(physics_obj other);
    virtual ~physics_obj();

 
    void integrate(float delta);

    inline const vec3f& get_velocity() const { return velocity; }

  
    collider* get_collider()
    {

        return _collider;
    }

    inline void set_velocity(const vec3f &velocity) { this->velocity = velocity; }

private:
    vec3f velocity;
    collider* _collider;
};


#endif //GAME_ENGINE_PHYSICS_OBJ_H
