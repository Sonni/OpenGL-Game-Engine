#include "core_engine.h"
#include "component/components.h"
#include "component/movement.h"
#include "component/water/water.h"
#include "component/shadow/shadow.h"
#include "component/text/text_component.h"
#include "component/particle/particle_system.h"
#include "component/mesh/advanced_mesh.h"
#include "component/mesh/simpel_mesh.h"
#include "frustum/frustum.h"
#include "component/mesh/animated_mesh.h"

core_engine::core_engine(window* Window, rendering_engine* renderingEngine, physics_engine* physicsEngine)
{
    this->_window = Window;
    this->renderingEngine = renderingEngine;
    this->physicsEngine = physicsEngine;
}


entity* create_mesh(std::string shader_name, vec3f pos, float scale, entity_component* ec)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->get_transform()->set_scale(scale);

    e->add_component(ec);
    return e;
}

entity* create_mesh_phong(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->get_shader()->add_light(lights);
    e->get_transform()->set_scale(scale);
    e->add_component(ec);
    return e;
}

entity* create_terrain(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec, std::vector<terrain_component*>* t)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->get_shader()->add_light(lights);
    e->get_transform()->set_scale(scale);
    e->add_component(ec);

    t->push_back((terrain_component*) ec);
    return e;
}

bool core_engine::run()
{
    float frameRate = 60;
    float m_frameTime = (float) (1.0 / frameRate);

    bool m_isRunning = true;

    double lastTime = (double) SDL_GetTicks()/1000.0;
    double frameCounter = 0;
    double unprocessedTime = 0;
    int frames = 0;

    ///////
    std::vector<entity*> entities, waterDraws, shadowDraws, shadowTmp, phy_obj;
    std::vector<terrain_component*> terrains;

    water_fbo wfb;
    mat4f shadow_mvp;
    texture depth_map(1024, 1024, true);
    texture post_processing(_window->WIN_WIDTH, _window->WIN_HEIGHT, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
    mesh* hud_mesh = renderingEngine->get_hud_mesh();

    phong_light l;
    l.s_pointLights.push_back(point_light(base_light(vec3f(1, 0, 0), 20.0f), attenuation(0, 0, 0.5), vec3f(30, 2, 30)));
    l.s_spotLights.push_back(spot_light(point_light(base_light(vec3f(0,0,1), 40.0f), attenuation(0,0,0.05f), vec3f(30, 12, 30)), ToRadians(45.f), quaternion(vec3f(1,0,0), ToRadians(-60.0f)) * quaternion(vec3f(0,1,0), ToRadians(90.0f))));
    //////


    float s = 10.0f;
    physics_obj* p_player = new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(s, s, s)));
    physics_obj* mk2 = new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(s, s, s)));

    vec2f blur;

    entity* terrain1 = create_terrain("testTerrain.glsl", vec3f(0, 0, 0), 1, &l, new terrain_component(&shadow_mvp, &depth_map, 0, 0, "terrain/empty.png", "terrain/grass.jpg", "terrain/flowers.jpg", "terrain/road.jpg", "terrain/blendmap.png"), &terrains);
    entity* terrain2 = create_terrain("testTerrain.glsl", vec3f(0, 0, 0), 1, &l, new terrain_component(&shadow_mvp, &depth_map, 0, 1, "terrain/empty.png", "terrain/grass.jpg", "terrain/flowers.jpg", "terrain/road.jpg", "terrain/blendmap.png"), &terrains);

    entity* post_p = create_mesh("gaussian_blur.glsl", vec3f(), 1, new blur_component(&post_processing, hud_mesh, &blur));
    entity* game_text = create_mesh("text.glsl", vec3f(), 1, new text_component("3D Game Engine", "arial", 3.0f, vec2f(0.0f, 0.0f), 1.0f, true));
    entity* box_animation = create_mesh_phong("advanced_mesh.glsl", vec3f(50, -1, 50), 1, &l, new advanced_mesh(new mesh("lantern"), &shadow_mvp, &depth_map, vec3f(0.3, 0.3, 0.3), mk2, "lantern.png", "bricks_normal.jpg", "default_disp.png", "lantern.png"));
    entity* monkey = create_mesh_phong("animation.glsl", vec3f(30, 45, 40), 1, &l, new animation_component("running_man", &shadow_mvp, &depth_map, p_player,  "", ""));
    entity* monkey2 = create_mesh_phong("simpel_mesh.glsl", vec3f(30, 65, 60), 5, &l, new advanced_mesh(new mesh("monkey"), &shadow_mvp, &depth_map, vec3f(0.3, 0.3, 0.3), mk2, "bricks.jpg"));
    entity* skybox = create_mesh("skybox.glsl", vec3f(), 2, new skybox_component());
    entity* health_bar = create_mesh("gui.glsl", vec3f(150, _window->get_height() - 150, 0), 0.3f, new gui_component("gui/healthBar.png", hud_mesh, _window->get_width(), _window->get_height()));
    entity* water = create_mesh("water.glsl", vec3f(80, 2000, 50), 1, new water_component("water/waterDUDV.png", "water/waterNormal.png", &wfb));
    entity* shadow = create_mesh("shadow.glsl", vec3f(), 1, new shadow_component(&shadowDraws, &depth_map, &shadow_mvp));
    entity* phong_mesh = create_mesh_phong("advanced_mesh.glsl", vec3f(30, 10, 30), 15, &l, new advanced_mesh(new mesh("plane"), &shadow_mvp, &depth_map, vec3f(0.3, 0.3, 0.3), new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(s, s, s))), "bricks.jpg", "bricks_normal.jpg", "bricks_disp.jpg"));


    mat4f* perspective = new mat4f();
    perspective->init_perspective(ToRadians(70.0f), _window->get_aspect(), 0.1f, 800.0f);

    frustum f;
    f.create_frustum(65.0f, _window->get_aspect(), 0.1f, 800.0);

    entity* Camera = new entity(NULL, vec3f(40, 40, 40));
    Camera->add_component(new camera_component(*perspective));
    Camera->add_component(new third_person(monkey->get_transform(), _window->get_center()));
    camera* cam = new camera(*perspective, Camera->get_transform(), &f);

    entity* _particle = create_mesh("particle.glsl", vec3f(40, 15, 40), 5, new particle_system("fire.png", 8, cam, 20, 25, 0.3f, 1.5f, 20.0f));


    entity* a[] = { Camera, monkey, box_animation, phong_mesh, terrain1, terrain2, skybox, water, monkey2, _particle, health_bar, game_text };
    entities.insert(entities.end(), a, a + 12);

    entity* b[] = { terrain1, terrain2, monkey, monkey2, box_animation, skybox };
    waterDraws.insert(waterDraws.end(), b, b + 6);

    entity* c[] = { phong_mesh, monkey, monkey2, box_animation };
    shadowDraws.insert(shadowDraws.end(), c, c + 4);

    entity* d[] = { monkey, monkey2 };
    phy_obj.insert(phy_obj.end(), d, d + 2);

    shadowTmp.push_back(shadow);


    //physics_obj* p_player = new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5), monkey->get_transform());

    std::vector<physics_obj*> physics_objs;
    physics_objs.push_back(mk2);
    //physics_objs.push_back(new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5), monkey2->get_transform()));
    //physics_objs.push_back(new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(s, s, s)), monkey2->get_transform()));





    while(m_isRunning)
    {
        bool render = false;

        double startTime = (double) SDL_GetTicks() / 1000.0;;
        double passedTime = startTime - lastTime; //Time passed since last frame
        lastTime = startTime;

        unprocessedTime += passedTime;
        frameCounter += passedTime;


        //update logic and IO
        while(unprocessedTime > m_frameTime)
        {
            _window->update();

            if(_window->is_closed_requested())
            {
                m_isRunning = false;
            }

            //update logic and input
            for (entity* e : entities)
            {
                e->process_input(_window->get_input(), m_frameTime);
                e->update(m_frameTime, *cam);
            }

            f.update_cam(*cam->get_transform()->get_pos(), cam->get_transform()->get_rot()->get_back(), vec3f(0,1,0));



            physicsEngine->update_terrain(terrains, phy_obj);
            physicsEngine->update_objs(p_player, physics_objs);
            physicsEngine->update_gravity(phy_obj, m_frameTime);

            render = true;
            unprocessedTime -= m_frameTime;
        }

        //update graphics
        if(render)
        {
            post_processing.bind_as_render_target();
            /// Render shadows
            shadow->set_all_uni(*cam);
            shadow->render();
            ///////////////////

            /// Render water
            float height_of_water = water->get_transform()->get_pos()->get_y();
            float y_distance = 2 * (cam->get_transform()->get_pos()->get_y() - height_of_water);
            quaternion old_cam_rot = cam->invert_pitch(*cam->get_transform()->get_rot());

            setup_reflection(cam, &wfb, height_of_water, y_distance);
            renderingEngine->render(cam, waterDraws);

            setup_refraction(cam, &wfb, height_of_water, y_distance, old_cam_rot);
            renderingEngine->render(cam, waterDraws);
            glDisable(GL_CLIP_DISTANCE0);
            cam->set_cutting_plane(vec4f(0, -1, 0, 10000000));
            /////////////////////////////////////////////////////////////////////////////

            /// Render scene
            post_processing.bind_as_render_target();
            renderingEngine->render(cam, entities);
            ////////////


            ///Render post processed tex to window
            _window->bind_as_render_targets();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            post_p->get_shader()->use_shader();

            blur.set(0.0 / _window->WIN_WIDTH, 0.0);
            post_p->set_all_uni(*cam);
            post_p->render();

            ////////////////////////////////////

            _window->swap_buffers();
            frames++;
        }
        else
        {
            SDL_Delay(10);
        }
    }

    //Clean up
    for (int i = 0; i < entities.size(); i++)
    {
        delete entities.at(i);
    }
    delete perspective;
    delete shadow;
    delete hud_mesh;
    delete cam;
    //if (FL) delete FL;
    //if (FM) delete FM;
    //if (TP) delete TP;

    return true;
}


void core_engine::setup_reflection(camera* cam, water_fbo* wfb, float height_of_water, float y_distance)
{
    glEnable(GL_CLIP_DISTANCE0);

    cam->set_cutting_plane(vec4f(0, 1, 0, -height_of_water + 1));

    wfb->bind_reflect_fbo();
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    cam->get_transform()->get_pos()->set_y(cam->get_transform()->get_pos()->get_y() - y_distance);
}

void core_engine::setup_refraction(camera* cam, water_fbo* wfb, float height_of_water, float y_distance, quaternion old_cam_rot)
{
    cam->get_transform()->get_pos()->set_y(cam->get_transform()->get_pos()->get_y() + y_distance);
    cam->reverse_pitch(old_cam_rot);

    cam->set_cutting_plane(vec4f(0, -1, 0, height_of_water));

    wfb->bind_refract_fbo();
    glClearColor(0.0f,0.0f,0.0f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


/*
 * if(Window->get_input().get_key_down(Window->get_input().KEY_4) && !wait)
            {
                Camera->delete_component(FL);
                Camera->delete_component(FM);

                Camera->add_component(TP);
                wait = true;
            }
            if(Window->get_input().get_key_down(Window->get_input().KEY_5) && wait)
            {
                Camera->delete_component(TP);

                Camera->add_component(FM);
                Camera->add_component(FL);
                wait = false;
            }

            /// day / night
            vec3f tmp = lights.at(0).get_color();
            tmp.set(tmp.get_x() + s, tmp.get_y() + s, tmp.get_z() + s);
            //lights.at(0).set_color(tmp);

            if (tmp.get_y() > 2) s = -0.001;
            if (tmp.get_y() < 0.2) s = 0.001;
            ///////////////////////
 */