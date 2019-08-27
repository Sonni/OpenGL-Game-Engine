#include "movement.hpp"

/////////////////// FREE MOVE ////////////////////
void free_move::process_input(const input& input, float delta)
{
    float movAmt = m_speed * delta;

    if(input.get_key_down(m_forward_key))
        move(get_transform()->get_rot()->get_forward(), movAmt);
    if(input.get_key_down(m_back_key))
        move(get_transform()->get_rot()->get_back(), movAmt);
    if(input.get_key_down(m_left_key))
        move(get_transform()->get_rot()->get_left(), movAmt);
    if(input.get_key_down(m_right_key))
        move(get_transform()->get_rot()->get_right(), movAmt);

    if(input.get_key_down(input.KEY_TAB))
        m_speed = 35;
}

void free_move::move(const vec3f &direction, float amt)
{
    get_transform()->set_pos(*get_transform()->get_pos() + (direction * amt));
}

/////////////////// FREE LOOK ////////////////////
vec2f old_pos;
void free_look::process_input(const input& input, float delta)
{
    if(input.get_key_down(m_unlock_mouse_key))
    {
        input.set_cursor(true);
        m_mouse_locked = false;
    }

    if(m_mouse_locked)
    {
        vec2f deltaPos = input.get_mouse_pos() - m_window_center;

        bool rotY = deltaPos.get_x() != 0;
        bool rotX = deltaPos.get_y() != 0;

        if(rotY)
        {
            get_transform()->rotate(vec3f(0, 1, 0), ToRadians(deltaPos.get_x() * m_sensitivity));
        }
        if(rotX)
        {
            get_transform()->rotate(get_transform()->get_rot()->get_right(), ToRadians(deltaPos.get_y() * m_sensitivity));
        }

        if(rotY || rotX)
        {
            input.set_mouse_pos(m_window_center);
        }
    }

    old_pos = input.get_mouse_pos();


    if(input.get_mouse_down(input::MOUSE_LEFT_BUTTON))
    {
        input.set_cursor(false);
        input.set_mouse_pos(m_window_center);
        m_mouse_locked = true;
    }

}

/////////////////// THIRD PERSON ////////////////////
void third_person::update(float delta, const camera &cam)
{
    m_yaw = (float) atan2(get_transform()->get_rot()->get_forward().get_x(), get_transform()->get_rot()->get_forward().get_z());
    float pitch = asinf(get_transform()->get_rot()->get_forward().get_y());

    float tmpH = (float) (m_distance_from_target * cos((pitch)));
    float tmpV = (float) (m_distance_from_target * sin((pitch)));


    get_transform()->get_pos()->set_y(m_target->get_pos()->get_y() - tmpV + 2);
    get_transform()->get_pos()->set_x((-tmpH * sin(m_yaw) + m_target->get_pos()->get_x()));
    get_transform()->get_pos()->set_z((-tmpH * cos(m_yaw) + m_target->get_pos()->get_z()));
}

void third_person::process_input(const input& input, float delta)
{

    if(input.get_mouse_up(input::MOUSE_LEFT_BUTTON))
    {
        //input.set_cursor(true);
        m_mouse_locked = false;
    }

    if(m_mouse_locked)
    {
        vec2f deltaPos = input.get_mouse_pos() - old_pos;

        bool rotY = deltaPos.get_x() != 0;
        bool rotX = deltaPos.get_y() != 0;

        if(rotY)
        {
            get_transform()->rotate(vec3f(0, 1, 0), deltaPos.get_x() * m_sensitivity * delta);
        }
        if(rotX)
        {
            get_transform()->rotate(get_transform()->get_rot()->get_right(), deltaPos.get_y() * m_sensitivity * delta);
        }

        if(rotY || rotX)
        {
            //input.set_mouse_pos(window_center);
        }
    }

    old_pos = input.get_mouse_pos();



    if(input.get_mouse_down(input::MOUSE_LEFT_BUTTON))
    {
        //input.set_cursor(false);
        //input.set_mouse_pos(window_center);
        m_mouse_locked = true;
    }


    float moveSpeed = 30.0f;
    if(input.get_key_down(input.KEY_W))
    {
        move_forward(moveSpeed, delta);
        m_target->set_rot(quaternion(vec3f(0, 1, 0), m_yaw));
    }
    if(input.get_key_down(input.KEY_S))
    {
        move_forward(-moveSpeed, delta);
        m_target->set_rot(quaternion(vec3f(0, 1, 0), m_yaw));
    }
    if(input.get_key_down(input.KEY_E))
        move_right(-moveSpeed, delta);
    if(input.get_key_down(input.KEY_Q))
        move_right(moveSpeed, delta);

    if(input.get_key_down(input.KEY_A))
    {
        get_transform()->rotate(vec3f(0, 1, 0), -moveSpeed * 0.2f * delta);
    }
    if(input.get_key_down(input.KEY_D))
    {
        get_transform()->rotate(quaternion(vec3f(0,1,0), moveSpeed * 0.2f * delta));
    }

    if(input.get_key_down(input.KEY_K))
        get_transform()->rotate(get_transform()->get_rot()->get_right(), moveSpeed * 0.2f * delta);

    if(input.get_key_down(input.KEY_L))
        get_transform()->rotate(get_transform()->get_rot()->get_right(), -moveSpeed * 0.2f * delta);

    if(input.get_key_down(input.KEY_SPACE))
        m_target->get_pos()->set_y(m_target->get_pos()->get_y() + moveSpeed * delta);

    if(input.get_key_down(input.KEY_V))
        m_target->get_pos()->set_y(m_target->get_pos()->get_y() - moveSpeed * delta);


    if (input.get_key_down(input.KEY_1))
        m_distance_from_target += 6.0 * delta;
    if (input.get_key_down(input.KEY_2))
        m_distance_from_target -= 6.0 * delta;


}

void third_person::move_forward(float amt, float delta)
{
    float distance = amt * delta;
    float tmpX = (float) (distance * sin(m_yaw)) + m_target->get_pos()->get_x();
    float tmpZ = (float) (distance * cos(m_yaw)) + m_target->get_pos()->get_z();

    float tmpY = m_target->get_pos()->get_y();


    m_target->get_pos()->set(tmpX, tmpY, tmpZ);
}

void third_person::move_right(float amt, float delta)
{
    m_target->get_pos()->set(*m_target->get_pos() - (get_transform()->get_rot()->get_right() * amt * delta));
}
