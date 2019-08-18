#ifndef ___D_rendering_engine__thirdPerson__
#define ___D_rendering_engine__thirdPerson__


#include "../entity_component.h"

/////////////////// FREE MOVE ////////////////////
class free_move : public entity_component
{
public:
    free_move(float speed = 10.0f, int forward_key = input::KEY_W, int back_key = input::KEY_S, int left_key = input::KEY_A, int right_key = input::KEY_D) :
            m_speed(speed),
            m_forward_key(forward_key),
            m_back_key(back_key),
            m_left_key(left_key),
            m_right_key(right_key)
    { }

    virtual void process_input(const input& input, float delta);
protected:
private:
    void move(const vec3f &direction, float amt);

    float m_speed;
    int m_forward_key;
    int m_back_key;
    int m_left_key;
    int m_right_key;
};

/////////////////// FREE LOOK ////////////////////
class free_look : public entity_component
{
public:
    free_look(const vec2f& window_center, float sensitivity = 0.5f, int unlock_mouse_key = input::KEY_ESCAPE) :
            m_sensitivity(sensitivity),
            m_unlock_mouse_key(unlock_mouse_key),
            m_mouse_locked(false),
            m_window_center(window_center)
    { }

    virtual void process_input(const input& input, float delta);
protected:
private:
    float m_sensitivity;
    int m_unlock_mouse_key;
    bool m_mouse_locked;
    vec2f m_window_center;

};

/////////////////// THIRD PERSON ////////////////////
class third_person : public entity_component
{
public:
    third_person(transform* target, const vec2f& window_center, float distance_from_target = 20, float sensitivity = 0.5f, int unlockMouseKey = input::KEY_ESCAPE) :
            m_sensitivity(sensitivity),
            m_unlock_mouse(unlockMouseKey),
            m_mouse_locked(false),
            m_window_center(window_center),
            m_distance_from_target(distance_from_target),
            m_yaw(0)
    {
        m_target = target;
    }

    virtual void process_input(const input& input, float delta);
    virtual void update(float delta, const camera &cam);
private:

    void move_forward(float amt, float delta);
    void move_right(float amt, float delta);


    transform* m_target;

    float m_distance_from_target, m_yaw;

    float m_sensitivity;
    int m_unlock_mouse;
    bool m_mouse_locked;
    vec2f m_window_center;
};


#endif /* defined(___D_rendering_engine__thirdPerson__) */