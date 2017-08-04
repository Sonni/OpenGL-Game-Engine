#include "core_engine.h"
#include "component/components.h"
#include "component/terrain.h"
#include "component/movement.h"
#include "component/water/water.h"
#include "component/shadow/shadow.h"

core_engine::core_engine(window* Window, rendering_engine* renderingEngine)
{
    this->Window = Window;
    this->renderingEngine = renderingEngine;
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

    std::vector<entity*> entities;
    std::vector<entity*> waterDraws;
    std::vector<entity*> shadowDraws;
    std::vector<entity*> shadowTmp;


    mat4f* perspective = new mat4f();
    perspective->init_perspective(ToRadians(70.0f), Window->get_aspect(), 0.1f, 1000.0f);




    ///////


    mat4f shadow_mvp;
    texture depth_map(1024, 1024, true);

    texture post_processing(Window->WIN_HEIGHT, Window->WIN_HEIGHT, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0);

    //////


    indexed_model hud_square;
    {
        hud_square.add_vertex(1.0f, -1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(1.0f, 1.0f));
        hud_square.add_vertex(1.0f, 1.0f, 0.0f);   hud_square.add_tex_coord(vec2f(1.0f, 0.0f));
        hud_square.add_vertex(-1.0f, -1.0f, 0.0f); hud_square.add_tex_coord(vec2f(0.0f, 1.0f));
        hud_square.add_vertex(-1.0f, 1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(0.0f, 0.0f));
        hud_square.add_face(0, 1, 2); hud_square.add_face(2, 1, 3);
    }
    mesh* hud_mesh = new mesh("squre", hud_square.finalize());

    vec2f blur;
    entity* post_p = new entity(new shader("gaussian_blur.glsl"));
    post_p->add_component(new blur_component(&post_processing, hud_mesh, &blur));


    entity* _particle = new entity(new shader("particle.glsl"), vec3f(40, 80, 40));
    _particle->add_component(new particle(hud_mesh, vec3f(0, 2, 0), 0.5f, 4.0, 50));
    _particle->get_transform()->set_scale(5);

    std::vector<light> lights;
    lights.push_back(light(vec3f(10000, 15000, -10000), vec3f(0.4, 0.4, 0.4))); //Sun
    lights.push_back(light(vec3f(30, 70, 40), vec3f(0, 0, 2.0), vec3f(0.2, 0.000001, 0.05))); //blue light
    lights.push_back(light(vec3f(40, 40, 40), vec3f(1.5, 0, 0), vec3f(0.6, 0.000001, 0.02))); //red light
    water_fbo wfb;

    entity* box_animation = new entity(new shader("animation.glsl"), vec3f(40, 45, 40));
    box_animation->add_component(new animation_component("box", &shadow_mvp, &depth_map));
    box_animation->get_shader()->add_lights(&lights);


    entity* monkey = new entity(new shader("basic.glsl"), vec3f(30, 45, 40));
    monkey->add_component(new mesh_component(new mesh("monkey"), &shadow_mvp, &depth_map, "default.jpg"));
    monkey->get_shader()->add_lights(&lights);
    monkey->get_transform()->set_scale(5);

    entity* monkey2 = new entity(new shader("basic_normal_mapping.glsl"), vec3f(30, 65, 40));
    monkey2->add_component(new mesh_component(new mesh("plane"), &shadow_mvp, &depth_map, "bricks.jpg", "bricks_normal.jpg"));
    monkey2->get_shader()->add_lights(&lights);
    monkey2->get_transform()->set_scale(20);

    entity* Camera = new entity(NULL, vec3f(40, 40, 40));
    camera* cam = new camera(*perspective, Camera->get_transform());

    third_person* TP = new third_person(monkey->get_transform(), Window->get_center());
    free_look* FL = new free_look(Window->get_center(), 0.4f);
    free_move* FM = new free_move(5.0f);

    Camera->add_component(new camera_component(*perspective));
    Camera->add_component(FL);
    Camera->add_component(FM);


    entity* terrain_mesh = new entity(new shader("testTerrain.glsl"));
    terrain_mesh->add_component(new terrain_component(&shadow_mvp, &depth_map, 0, 0, "terrain/empty.png", "terrain/grass.jpg", "terrain/flowers.jpg", "terrain/road.jpg", "terrain/blendmap.png"));
    terrain_mesh->get_shader()->add_lights(&lights);

    entity* skybox = new entity(new shader("skybox.glsl"));
    skybox->add_component(new skybox_component(500));

    entity* health_bar = new entity(new shader("gui.glsl"), vec3f(150, Window->get_height() - 150, 0));
    health_bar->get_transform()->set_scale(0.3);
    health_bar->add_component(new gui_component("gui/healthBar.png", hud_mesh, Window->get_width(), Window->get_height()));



    entity* water = new entity(new shader("water.glsl"), vec3f(80, 40, 50));
    water->add_component(new water_component("water/waterDUDV.png", "water/waterNormal.png", &wfb));
    water->get_transform()->set_scale(500);

    entity* shadow = new entity(new shader("shadow.glsl"));
    shadow->add_component(new shadow_component(&shadowDraws, &depth_map, &shadow_mvp));

    shadowTmp.push_back(shadow);

    entities.push_back(Camera);
    entities.push_back(monkey);
    entities.push_back(box_animation);
    entities.push_back(terrain_mesh);
    entities.push_back(skybox);
    entities.push_back(water);
    entities.push_back(monkey2);

    entities.push_back(_particle);
    entities.push_back(health_bar);


    waterDraws.push_back(terrain_mesh);
    waterDraws.push_back(monkey);
    waterDraws.push_back(monkey2);
    waterDraws.push_back(box_animation);
    waterDraws.push_back(skybox);

    shadowDraws.push_back(monkey);
    shadowDraws.push_back(monkey2);
    shadowDraws.push_back(box_animation);

    float s = 0.001;
    bool wait = false;
    while(m_isRunning)
    {
        bool render = false;

        double startTime = (double) SDL_GetTicks()/1000.0;;
        double passedTime = startTime - lastTime; //Time passed since last frame
        lastTime = startTime;

        unprocessedTime += passedTime;
        frameCounter += passedTime;


        //update logic and IO
        while(unprocessedTime > m_frameTime)
        {
            Window->update();

            if(Window->is_closed_requested())
            {
                m_isRunning = false;
            }

            if(Window->get_input().get_key_down(Window->get_input().KEY_4) && !wait)
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

            //update logic and input
            for (entity* e : entities)
            {
                e->process_input(Window->get_input(), m_frameTime);
                e->update(m_frameTime);
            }


            vec3f tmp = lights.at(0).get_color();
            tmp.set(tmp.get_x() + s, tmp.get_y() + s, tmp.get_z() + s);
            lights.at(0).set_color(tmp);

            if (tmp.get_y() > 2) s = -0.001;
            if (tmp.get_y() < 0.2) s = 0.001;


            render = true;
            unprocessedTime -= m_frameTime;
        }

        //update graphics
        if(render)
        {
            post_processing.bind_as_render_target();
            /////////////////


            shadow->set_all_uni(*cam);
            shadow->render();

            ///////////////////

            float waterHeight = water->get_transform()->get_pos()->get_y();
            glEnable(GL_CLIP_DISTANCE0);

            cam->set_cutting_plane(vec4f(0, 1, 0, -waterHeight+1));

            wfb.bind_reflect_fbo();
            glClearColor(0.0f,0.0f,0.0f,0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            camera tmpCam = *cam;
            float tmpDistance = 2 * (tmpCam.get_transform()->get_pos()->get_y() - waterHeight);
            tmpCam.get_transform()->get_pos()->set_y(tmpCam.get_transform()->get_pos()->get_y() - tmpDistance);

            quaternion oldPitch = tmpCam.invert_pitch(*cam->get_transform()->get_rot());
            renderingEngine->render(cam, waterDraws);

            tmpCam.get_transform()->get_pos()->set_y(tmpCam.get_transform()->get_pos()->get_y() + tmpDistance);
            tmpCam.reverse_pitch(oldPitch);

            cam->set_cutting_plane(vec4f(0, -1, 0, waterHeight));

            wfb.bind_refract_fbo();
            glClearColor(0.0f,0.0f,0.0f,0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderingEngine->render(cam, waterDraws);
            glDisable(GL_CLIP_DISTANCE0);

            cam->set_cutting_plane(vec4f(0, -1, 0, 10000000));

            post_processing.bind_as_render_target();

            renderingEngine->render(cam, entities);

            Window->bind_as_render_targets();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            post_p->get_shader()->use_shader();

            blur.set(0.0/Window->WIN_WIDTH, 0.0);
            post_p->set_all_uni(*cam);
            post_p->render();

            /*blur.set(0.0, 4.0/Window->WIN_HEIGHT);
            post_p->set_all_uni(*cam);
            post_p->render();*/

            Window->swap_buffers();
            frames++;
        }
        else
        {
            SDL_Delay(1);
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


