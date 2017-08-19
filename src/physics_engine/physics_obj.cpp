#include "physics_obj.h"

physics_obj::physics_obj(const physics_obj& other) :
        velocity(other.velocity),
        _collider(other._collider)
{
}

void physics_obj::operator=(physics_obj other)
{
    char* temp[sizeof(physics_obj)/sizeof(char)];
    memcpy(temp, this, sizeof(physics_obj));
    memcpy(this, &other, sizeof(physics_obj));
    memcpy(&other, temp, sizeof(physics_obj));
}

physics_obj::~physics_obj()
{

}


void physics_obj::integrate(float delta)
{
}