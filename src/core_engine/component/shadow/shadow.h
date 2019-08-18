#ifndef GAME_ENGINE_SHADOW_MAPPING_H
#define GAME_ENGINE_SHADOW_MAPPING_H


#include <GL/glew.h>
#include "../components.h"


class shadow_component : public entity_component
{
public:
    shadow_component(std::vector<entity*>* entities, texture* depth_map, mat4f* shadow_mvp, int shadowMapSizeAsPowerOf2 = 10, float shadowArea = 80.0f);

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void render() const;

private:
    GLint m_mvp_loc;
    mat4f m_BIAS_MATRIX;
    mat4f m_light_view_projection;
    mat4f* m_shadow_mvp;
    texture* m_depth_map;

    std::vector<entity*>* m_entities;

    float m_half_shadow_area;
    int m_shadow_map_power_2;
    mat4f m_orto_projection;


    transform get_shadow_transform(const transform &cam_transform) const;
};


#endif //GAME_ENGINE_SHADOW_MAPPING_H
