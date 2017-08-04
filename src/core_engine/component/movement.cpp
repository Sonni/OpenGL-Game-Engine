#include "movement.h"

/////////////////// FREE MOVE ////////////////////
void free_move::process_input(const input& input, float delta)
{
    float movAmt = speed * delta;

    if(input.get_key_down(forward_key))
        move(get_transform()->get_rot()->get_forward(), movAmt);
    if(input.get_key_down(back_key))
        move(get_transform()->get_rot()->get_back(), movAmt);
    if(input.get_key_down(left_key))
        move(get_transform()->get_rot()->get_left(), movAmt);
    if(input.get_key_down(right_key))
        move(get_transform()->get_rot()->get_right(), movAmt);

    if(input.get_key_down(input.KEY_TAB))
        speed = 35;
}

void free_move::move(const vec3f &direction, float amt)
{
    get_transform()->set_pos(*get_transform()->get_pos() + (direction * amt));
}

/////////////////// FREE LOOK ////////////////////
void free_look::process_input(const input& input, float delta)
{
    if(input.get_key_down(unlock_mouse_key))
    {
        input.set_cursor(true);
        mouse_locked = false;
    }

    if(mouse_locked)
    {
        vec2f deltaPos = input.get_mouse_pos() - window_center;

        bool rotY = deltaPos.get_x() != 0;
        bool rotX = deltaPos.get_y() != 0;

        if(rotY)
        {
            get_transform()->rotate(vec3f(0, 1, 0), ToRadians(deltaPos.get_x() * sensitivity));
        }
        if(rotX)
        {
            get_transform()->rotate(get_transform()->get_rot()->get_right(), ToRadians(deltaPos.get_y() * sensitivity));
        }

        if(rotY || rotX)
        {
            input.set_mouse_pos(window_center);
        }
    }

    if(input.get_mouse_down(input::MOUSE_LEFT_BUTTON))
    {
        input.set_cursor(false);
        input.set_mouse_pos(window_center);
        mouse_locked = true;
    }
}

/////////////////// THIRD PERSON ////////////////////
void third_person::update(float delta)
{
    yaw = (float) atan2(get_transform()->get_rot()->get_forward().get_x(), get_transform()->get_rot()->get_forward().get_z());
    float pitch = asinf(get_transform()->get_rot()->get_forward().get_y());

    float tmpH = (float) (distance_from_target * cos((pitch)));
    float tmpV = (float) (distance_from_target * sin((pitch)));


    get_transform()->get_pos()->set_y(target->get_pos()->get_y() - tmpV - 2);
    get_transform()->get_pos()->set_x((-tmpH * sin(yaw) + target->get_pos()->get_x()));
    get_transform()->get_pos()->set_z((-tmpH * cos(yaw) + target->get_pos()->get_z()));


}

void third_person::process_input(const input& input, float delta)
{

    if(input.get_key_down(unlock_mouse))
    {
        input.set_cursor(true);
        mouse_locked = false;
    }

    if(mouse_locked)
    {
        vec2f deltaPos = input.get_mouse_pos() - window_center;

        bool rotY = deltaPos.get_x() != 0;
        bool rotX = deltaPos.get_y() != 0;

        if(rotY)
        {
            get_transform()->rotate(vec3f(0, 1, 0), deltaPos.get_x() * sensitivity * delta);
        }
        if(rotX)
        {
            get_transform()->rotate(get_transform()->get_rot()->get_right(), deltaPos.get_y() * sensitivity * delta);
        }

        if(rotY || rotX)
        {
            input.set_mouse_pos(window_center);
        }
    }


    if(input.get_mouse_down(input::MOUSE_LEFT_BUTTON))
    {
        input.set_cursor(false);
        input.set_mouse_pos(window_center);
        mouse_locked = true;
    }


    float moveSpeed = 20.0f;
    if(input.get_key_down(input.KEY_W))
        move_forward(moveSpeed, delta);
    if(input.get_key_down(input.KEY_S))
        move_forward(-moveSpeed, delta);
    if(input.get_key_down(input.KEY_E))
        move_right(-moveSpeed, delta);
    if(input.get_key_down(input.KEY_Q))
        move_right(moveSpeed, delta);

    if(input.get_key_down(input.KEY_A))
        get_transform()->rotate(vec3f(0, 1, 0), -moveSpeed * 0.2f * delta);

    if(input.get_key_down(input.KEY_D))
        get_transform()->rotate(quaternion(vec3f(0,1,0), moveSpeed * 0.2f * delta));

    if(input.get_key_down(input.KEY_K))
        get_transform()->rotate(get_transform()->get_rot()->get_right(), moveSpeed * 0.2f * delta);

    if(input.get_key_down(input.KEY_L))
        get_transform()->rotate(get_transform()->get_rot()->get_right(), -moveSpeed * 0.2f * delta);

    if(input.get_key_down(input.KEY_SPACE))
        target->get_pos()->set_y(target->get_pos()->get_y() + moveSpeed * delta);

    if(input.get_key_down(input.KEY_V))
        target->get_pos()->set_y(target->get_pos()->get_y() - moveSpeed * delta);


    if (input.get_key_down(input.KEY_1))
        distance_from_target += 6.0 * delta;
    if (input.get_key_down(input.KEY_2))
        distance_from_target -= 6.0 * delta;


}

void third_person::move_forward(float amt, float delta)
{
    float distance = amt * delta;
    float tmpX = (float) (distance * sin(yaw)) + target->get_pos()->get_x();
    float tmpZ = (float) (distance * cos(yaw)) + target->get_pos()->get_z();

    float tmpY = target->get_pos()->get_y();


    target->get_pos()->set(tmpX, tmpY, tmpZ);
}

void third_person::move_right(float amt, float delta)
{
    target->get_pos()->set(*target->get_pos() - (get_transform()->get_rot()->get_right() * amt * delta));
}
