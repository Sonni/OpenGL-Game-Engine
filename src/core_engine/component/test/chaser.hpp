#ifndef INC_3D_ENGINE_CHASER_HPP
#define INC_3D_ENGINE_CHASER_HPP


#include "../../entity_component.hpp"

class chaser_component : public entity_component
{
public:
    chaser_component(transform* target)
    {
    this->target = target;
    }

    virtual void update(float delta, const camera &cam)
    {
        vec3f direction = *target->get_pos() - *get_transform()->get_pos();
        direction = direction.normalized() * SPEED * delta;
        vec3f new_pos = *get_transform()->get_pos() + direction;


        float distance = (*target->get_pos() - *get_transform()->get_pos()).length();
        if (distance < target_radius)
        {
            return;
        }

        if (distance > slow_radius)
        {
            target_speed = SPEED;
        }
        else
        {
            target_speed = SPEED * distance / slow_radius;
        }

        get_transform()->set_pos(vec3f(new_pos.get_x(), get_transform()->get_pos()->get_y(), new_pos.get_z()));

    }

private:
    float SPEED = 14.0;
    transform* target;


    float max_acceleration = 0.2;
    float target_radius = 5.0;
    float slow_radius = 10.0;
    float time_to_target = 10.0;
    float target_speed = 20.0;

};


#endif //INC_3D_ENGINE_CHASER_HPP