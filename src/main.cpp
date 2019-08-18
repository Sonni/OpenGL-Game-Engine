#include <iostream>
#include "core_engine/core_engine.h"
#include "core_engine/io/sound.h"
#include "core_engine/component/text/text_component.h"
#include "core_engine/component/particle/particle_system.h"
#include "core_engine/component/components.h"
#include "core_engine/component/movement.h"
#include "core_engine/component/mesh/advanced_mesh.h"
#include "core_engine/component/mesh/animated_mesh.h"
#include "core_engine/component/test/chaser.h"

int main(int argc, char *argv[])
{
    window* main_window = new window(1440, 900, "Sonni's 3D Game Engine");
    rendering_engine* graphics_engine = new rendering_engine();
    physics_engine* physicsEngine = new physics_engine();
    core_engine* _core_engine = new core_engine(main_window, graphics_engine, physicsEngine);

    physics_obj* p_player = new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 3));
    physics_obj* mk2 = new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5));
    std::vector<terrain_component*> terrains;

    water_fbo wfb;
    mat4f shadow_mvp;
    texture depth_map(1024, 1024, true);
    texture post_processing(main_window->WIN_WIDTH, main_window->WIN_HEIGHT, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
    mesh* hud_mesh = graphics_engine->get_hud_mesh();

    phong_light Phong_light;
    Phong_light.m_pointLights.push_back(point_light(base_light(vec3f(1, 0, 0), 20.0f), attenuation(0, 0, 0.5), vec3f(30, 2, 30)));
    Phong_light.m_spotLights.push_back(spot_light(point_light(base_light(vec3f(0,0,1), 40.0f), attenuation(0,0,0.05f), vec3f(30, 12, 30)), ToRadians(45.f), quaternion(vec3f(1,0,0), ToRadians(-60.0f)) * quaternion(vec3f(0,1,0), ToRadians(90.0f))));
    //////

    entity* character = _core_engine->create_mesh_phong("animation.glsl", vec3f(30, 45, 40), 1, &Phong_light, new animation_component("running_man", &shadow_mvp, &depth_map, p_player,  "", ""));


    mat4f* perspective = new mat4f();
    perspective->init_perspective(ToRadians(70.0f), main_window->get_aspect(), 0.1f, 800.0f);

    frustum Frustum;
    Frustum.create_frustum(ToRadians(65.0f), main_window->get_aspect(), 0.1f, 800.0);

    entity* Camera = new entity(NULL, vec3f(10, 40, 10));
    Camera->add_component(new camera_component(*perspective));
    //Camera->add_component(new third_person(character->get_transform(), main_window->get_center()));
    Camera->add_component(new free_look(main_window->get_center())); Camera->add_component(new free_move(50));

    camera* cam = new camera(*perspective, Camera->get_transform(), &Frustum);

    entity* terrain1 = _core_engine->create_terrain("terrain_normal_map.glsl", vec3f(0, 0, 0), 1, &Phong_light, new terrain_component(0, 0, 512, "terrain/grass.png", "terrain/grass.png", "terrain/grass.png", "terrain/road.png", "terrain/blendmap.png", &shadow_mvp, &depth_map), &terrains);
    //entity* terrain2 = _core_engine->create_terrain("testTerrain.glsl", vec3f(0, 0, 0), 1, &Phong_light, new terrain_component(&shadow_mvp, &depth_map, 0, 1, "terrain/empty.png", "terrain/grass.jpg", "terrain/flowers.jpg", "terrain/road.jpg", "terrain/blendmap.png"), &terrains);


    entity* game_text = _core_engine->create_mesh("text.glsl", vec3f(), 1, new text_component("3D Game Engine By Sonni", "arial", 3.0f, vec2f(0.0f, 0.0f), 1.0f, true));
    entity* health_bar = _core_engine->create_mesh("gui.glsl", vec3f(150, main_window->get_height() - 150, 0), 0.3f, new gui_component("gui/healthBar.png", hud_mesh, main_window->get_width(), main_window->get_height()));
    entity* monkey2 = _core_engine->create_mesh_phong("simpel_mesh.glsl", vec3f(30, 65, 60), 5, &Phong_light, new advanced_mesh(new mesh("monkey"), &shadow_mvp, &depth_map, vec3f(0.3, 0.3, 0.3), mk2, "bricks.jpg"));

    entity* _particle = _core_engine->create_mesh("particle.glsl", vec3f(40, 15, 40), 5, new particle_system("fire.png", 8, cam, 20, 25, 0.3f, 1.5f, 20.0f));

    entity* phong_mesh = _core_engine->create_mesh_phong("advanced_mesh.glsl", vec3f(30, 10, 30), 15, &Phong_light, new advanced_mesh(new mesh("plane"), &shadow_mvp, &depth_map, vec3f(0.3, 0.3, 0.3), new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(10.0, 10.0, 10.0))), "bricks.jpg", "bricks_normal.jpg", "bricks_disp.jpg"));

    entity* chaser = _core_engine->create_mesh_phong("advanced_mesh.glsl", vec3f(50, -1, 50), 3, &Phong_light, new advanced_mesh(new mesh("monkey"), &shadow_mvp, &depth_map, vec3f(0.3, 0.3, 0.3), mk2, "lantern.png", "bricks_normal.jpg", "default_disp.png", "lantern.png"));
    chaser->add_component(new chaser_component(character->get_transform()));

    entity* skybox = _core_engine->create_mesh("skybox.glsl", vec3f(), 2, new skybox_component());





    _core_engine->set_camera(cam, Camera);
    _core_engine->set_frustum(&Frustum);

    _core_engine->add_entity(skybox);
    _core_engine->add_entity(terrain1);
    //_core_engine->add_entity(terrain2);
    /*_core_engine->add_entity(phong_mesh);
    _core_engine->add_entity(monkey2);
    _core_engine->add_entity(_particle);
    _core_engine->add_entity(game_text);
    _core_engine->add_entity(health_bar);
    _core_engine->add_entity(character);
    _core_engine->add_entity(chaser);

    _core_engine->add_physics_obj(monkey2);
    _core_engine->add_physics_obj(character);

    _core_engine->add_water_and_shadow(phong_mesh);
    _core_engine->add_water_and_shadow(monkey2);
    _core_engine->add_water_and_shadow(character);

    _core_engine->add_water_reflect(terrain1);
    _core_engine->add_water_reflect(terrain2);
    _core_engine->add_water_reflect(skybox);*/

    //_core_engine->add_water_reflect(terrain1);
    _core_engine->add_water_reflect(skybox);
    _core_engine->add_water_and_shadow(terrain1);


    _core_engine->add_entity(Camera);


    _core_engine->run();

    delete perspective;
    delete main_window;
    delete graphics_engine;
    delete _core_engine;
    delete physicsEngine;

    return 0;
}