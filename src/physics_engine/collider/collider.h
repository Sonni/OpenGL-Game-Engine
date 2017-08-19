#ifndef GAME_ENGINE_COLLIDER_H
#define GAME_ENGINE_COLLIDER_H


#include "../intersect_data.h"

class collider
{
public:

    enum
    {
        TYPE_SPHERE,
        TYPE_AABB,

        TYPE_SIZE
    };


    collider(int type) :
            type(type) {}


    intersect_data intersect(const collider &other) const;


    virtual void transform(const vec3f &translation) {}
    virtual void set_pos(const vec3f &pos) {}
    virtual vec3f get_center() const { return vec3f(0,0,0); }

    inline int get_type() const { return type; }
private:
    int type;
};


#endif //GAME_ENGINE_COLLIDER_H
