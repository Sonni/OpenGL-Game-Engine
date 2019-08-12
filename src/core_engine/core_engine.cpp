#include "core_engine.h"
#include "component/water/water.h"
#include "component/shadow/shadow.h"

#include "../client/old/TCPClient.h"
#include <thread>

core_engine::core_engine(window* Window, rendering_engine* renderingEngine, physics_engine* physicsEngine)
{
    this->_window = Window;
    this->renderingEngine = renderingEngine;
    this->physicsEngine = physicsEngine;
}


entity* core_engine::create_mesh(std::string shader_name, vec3f pos, float scale, entity_component* ec)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->get_transform()->set_scale(scale);

    e->add_component(ec);
    return e;
}

entity* core_engine::create_mesh_phong(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->get_shader()->add_light(lights);
    e->get_transform()->set_scale(scale);
    e->add_component(ec);
    return e;
}

entity* core_engine::create_terrain(std::string shader_name, vec3f pos, float scale, phong_light* lights, entity_component* ec, std::vector<terrain_component*>* t)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->get_shader()->add_light(lights);
    e->get_transform()->set_scale(scale);
    e->add_component(ec);

    t->push_back((terrain_component*) ec);
    return e;
}

void core_engine::add_entity(entity *Entity)
{
    entities.push_back(Entity);
}

void core_engine::add_physics_obj(entity *Entity)
{
    phy_obj.push_back(Entity);
}

void core_engine::add_water_and_shadow(entity *Entity)
{
    waterDraws.push_back(Entity);
    shadowDraws.push_back(Entity);
}

void core_engine::add_water_reflect(entity *Entity)
{
    waterDraws.push_back(Entity);
}

void core_engine::set_camera(camera *cam, entity *Camera)
{
    this->cam = cam;
    this->Camera = Camera;
}


void core_engine::set_frustum(frustum *f)
{
    Frustum = f;
}

bool core_engine::run()
{


    ///////
    std::vector<entity*> shadowTmp;
    std::vector<terrain_component*> terrains;

    water_fbo wfb;
    mat4f shadow_mvp;
    texture depth_map(1024, 1024, true);
    texture post_processing(_window->WIN_WIDTH, _window->WIN_HEIGHT, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
    mesh* hud_mesh = renderingEngine->get_hud_mesh();

    //////


    physics_obj* mk2 = new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5));

    vec2f blur;


    entity* post_p = create_mesh("gaussian_blur.glsl", vec3f(), 1, new blur_component(&post_processing, hud_mesh, &blur));
    entity* water = create_mesh("water.glsl", vec3f(80, 200, 50), 500, new water_component("water/waterDUDV.png", "water/waterNormal.png", &wfb));
    entity* shadow = create_mesh("shadow.glsl", vec3f(), 1, new shadow_component(&shadowDraws, &depth_map, &shadow_mvp));






    entity* a[] = { Camera, water };
    entities.insert(entities.begin(), a, a + 2);



    shadowTmp.push_back(shadow);



    std::vector<physics_obj*> physics_objs;
    physics_objs.push_back(mk2);
    //physics_objs.push_back(new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5), monkey2->get_transform()));
    //physics_objs.push_back(new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(s, s, s)), monkey2->get_transform()));


   /* if (SDLNet_Init() == -1)
    {
        printf("SDLNet_Init: %s\n", SDLNet_GetError());
        exit(2);
    }

    TCPClient tcpClient("127.0.0.1", 3400);
    std::thread t1(&TCPClient::start, &tcpClient);*/


    float FPS = 30;
    float m_frameTime = 1000.0 / FPS;
    float updateRate = 60;
    float m_updateTime = (float) (1.0 / updateRate);

    bool m_isRunning = true;

    double lastTimeSec = (double) SDL_GetTicks()/1000.0;
    double frameCounter = 0;
    double unprocessedTime = 0;
    int frames = 0;

    float secondCounter = 0.0;
    int updates;
    while(m_isRunning)
    {
        double startTimeSec = (double) SDL_GetTicks() / 1000.0; // Time in seconds
        double passedTimeSec = startTimeSec - lastTimeSec; //Time passed since last frame
        double passedTimeMs = passedTimeSec * 1000; //Time passed since last frame
        lastTimeSec = startTimeSec;

        unprocessedTime += passedTimeSec;
        frameCounter += passedTimeMs;
        secondCounter += passedTimeMs;

        //update logic and IO
        while(unprocessedTime > m_updateTime)
        {
            _window->update();

            if(_window->is_closed_requested())
            {
                m_isRunning = false;
            }

            //update logic and input
            for (entity* e : entities)
            {
                e->process_input(_window->get_input(), m_updateTime);
                e->update(m_updateTime, *cam);
            }

            Frustum->update_cam(*cam->get_transform()->get_pos(), cam->get_transform()->get_rot()->get_back(), vec3f(0,1,0));

            physicsEngine->update_terrain(terrains, phy_obj);
            //physicsEngine->update_objs(monkey, p_player, physics_objs);
            physicsEngine->update_gravity(phy_obj, m_updateTime);

            unprocessedTime -= m_updateTime;
            updates++;
        }

        if (secondCounter > 1000)
        {
            //std::cout << "FPS: " << frames << " Updates: " << updates << std::endl;

            frames = updates = 0;
            secondCounter = 0.0;

            std::string pos = "<M> " + std::to_string(cam->get_transform()->get_pos()->get_x()) + " " + std::to_string(cam->get_transform()->get_pos()->get_y()) + " " + std::to_string(cam->get_transform()->get_pos()->get_z());

            //tcpClient.send(pos);
        }


        //update graphics
        if(frameCounter > m_frameTime)
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
            frameCounter -= m_frameTime;
        }
        else
        {
            SDL_Delay(15);
        }
    }

    //Clean up
    for (int i = 0; i < entities.size(); i++)
    {
        delete entities.at(i);
    }
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
