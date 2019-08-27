#ifndef GAME_ENGINE_CORE_H
#define GAME_ENGINE_CORE_H

#include "io/window.hpp"
#include "../rendering_engine/rendering_engine.hpp"
#include "component/water/water_fbo.h"
#include "../physics_engine/physics_engine.hpp"
#include "frustum/frustum.hpp"
#include "../rendering_engine/light/phong_light.hpp"

class core_engine
{
public:
    core_engine(window* Window, rendering_engine* renderingEngine, physics_engine* physicsEngine);
    bool run();
    void add_entity(entity *Entity);
    void add_physics_obj(entity *Entity);
    void add_water_and_shadow(entity *Entity);
    void add_water_reflect(entity *Entity);
    void add_scatter(entity *Entity);
    entity* create_mesh(std::string shader_name, vec3f pos, float scale, entity_component* ec);
    entity* create_mesh_phong(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec);
    entity* create_terrain(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec, std::vector<terrain_component*>* t);

    void set_camera(camera *cam, entity *Camera);
    void set_frustum(frustum *f);

private:
    window* m_window;
    rendering_engine* m_renderingEngine;
    physics_engine* m_physicsEngine;
    std::vector<entity*> m_entities;
    std::vector<entity*> m_phy_obj;

    std::vector<entity*> m_waterDraws;
    std::vector<entity*> m_shadowDraws;
    std::vector<entity*> m_scatters;
    camera* m_cam;
    frustum* m_Frustum;
    entity* m_Camera;

    void setup_reflection(camera* cam, water_fbo* wfb, float height_of_water, float y_distance);
    void setup_refraction(camera* cam, water_fbo* wfb, float height_of_water, float y_distance, quaternion old_cam_rot);
};


#endif //GAME_ENGINE_CORE_H
