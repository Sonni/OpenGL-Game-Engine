#ifndef GAME_ENGINE_CORE_H
#define GAME_ENGINE_CORE_H

#include "io/window.h"
#include "../rendering_engine/rendering_engine.h"
#include "component/water/water_fbo.h"
#include "../physics_engine/physics_engine.h"
#include "frustum/frustum.h"
#include "../rendering_engine/light/phong_light.h"

class core_engine
{
public:
    core_engine(window* Window, rendering_engine* renderingEngine, physics_engine* physicsEngine);
    bool run();
    void add_entity(entity *Entity);
    void add_physics_obj(entity *Entity);
    void add_water_and_shadow(entity *Entity);
    void add_water_reflect(entity *Entity);
    entity* create_mesh(std::string shader_name, vec3f pos, float scale, entity_component* ec);
    entity* create_mesh_phong(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec);
    entity* create_terrain(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec, std::vector<terrain_component*>* t);

    void set_camera(camera *cam, entity *Camera);
    void set_frustum(frustum *f);

private:
    window* _window;
    rendering_engine* renderingEngine;
    physics_engine* physicsEngine;
    std::vector<entity*> entities;
    std::vector<entity*> phy_obj;

    std::vector<entity*> waterDraws;
    std::vector<entity*> shadowDraws;
    camera* cam;
    frustum* Frustum;
    entity* Camera;

    void setup_reflection(camera* cam, water_fbo* wfb, float height_of_water, float y_distance);
    void setup_refraction(camera* cam, water_fbo* wfb, float height_of_water, float y_distance, quaternion old_cam_rot);
};


#endif //GAME_ENGINE_CORE_H
