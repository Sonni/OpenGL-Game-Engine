#ifndef GAME_ENGINE_LIGHT_H
#define GAME_ENGINE_LIGHT_H


#include "../core_engine/math/math_lib.h"

class light
{
public:
    light(const vec3f& pos = vec3f(0, 0, 0), const vec3f& color = vec3f(0, 0, 0), const vec3f& attenuation = vec3f(1, 0, 0)) :
            pos(pos),
            color(color),
            attenuation(attenuation)
    { }

    inline const vec3f& get_pos() const { return pos; }
    inline const vec3f& get_color() const { return color; }
    inline const vec3f& get_attenuation() const { return attenuation; }

    inline void set_color(const vec3f& color) {this->color = color;}

private:
    vec3f pos, color, attenuation;
};


#endif //GAME_ENGINE_LIGHT_H
