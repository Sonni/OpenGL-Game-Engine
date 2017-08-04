#ifndef ___D_rendering_engine__thirdPerson__
#define ___D_rendering_engine__thirdPerson__


#include "../entity_component.h"

/////////////////// FREE MOVE ////////////////////
class free_move : public entity_component
{
public:
    free_move(float speed = 10.0f, int forward_key = input::KEY_W, int back_key = input::KEY_S, int left_key = input::KEY_A, int right_key = input::KEY_D) :
            speed(speed),
            forward_key(forward_key),
            back_key(back_key),
            left_key(left_key),
            right_key(right_key)
    { }

    virtual void process_input(const input& input, float delta);
protected:
private:
    void move(const vec3f &direction, float amt);

    float speed;
    int forward_key;
    int back_key;
    int left_key;
    int right_key;
};

/////////////////// FREE LOOK ////////////////////
class free_look : public entity_component
{
public:
    free_look(const vec2f& window_center, float sensitivity = 0.5f, int unlock_mouse_key = input::KEY_ESCAPE) :
            sensitivity(sensitivity),
            unlock_mouse_key(unlock_mouse_key),
            mouse_locked(false),
            window_center(window_center)
    { }

    virtual void process_input(const input& input, float delta);
protected:
private:
    float sensitivity;
    int unlock_mouse_key;
    bool mouse_locked;
    vec2f window_center;

};

/////////////////// THIRD PERSON ////////////////////
class third_person : public entity_component
{
public:
    third_person(transform* target, const vec2f& window_center, float distance_from_target = 20, float sensitivity = 0.5f, int unlockMouseKey = input::KEY_ESCAPE) :
            sensitivity(sensitivity),
            unlock_mouse(unlockMouseKey),
            mouse_locked(false),
            window_center(window_center),
            distance_from_target(distance_from_target),
            yaw(0)
    {
        this->target = target;
    }

    virtual void process_input(const input& input, float delta);
    virtual void update(float delta);
private:

    void move_forward(float amt, float delta);
    void move_right(float amt, float delta);


    transform* target;

    float distance_from_target, yaw;

    float sensitivity;
    int unlock_mouse;
    bool mouse_locked;
    vec2f window_center;
};


#endif /* defined(___D_rendering_engine__thirdPerson__) */