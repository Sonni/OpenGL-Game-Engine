#include "physics_obj.hpp"

physics_obj::physics_obj(const physics_obj& other) :
        velocity(other.velocity),
        _collider(other._collider)
{
}

void physics_obj::operator=(physics_obj other)
{

}

physics_obj::~physics_obj()
{

}


void physics_obj::integrate(float delta)
{
}