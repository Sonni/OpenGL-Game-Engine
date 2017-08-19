#include "core_engine.h"
#include "component/components.h"
#include "component/terrain.h"
#include "component/movement.h"
#include "component/water/water.h"
#include "component/shadow/shadow.h"
#include "component/text/text_component.h"
#include "../physics_engine/collider/sphere.h"

core_engine::core_engine(window* Window, rendering_engine* renderingEngine, physics_engine* physicsEngine)
{
    this->Window = Window;
    this->renderingEngine = renderingEngine;
    this->physicsEngine = physicsEngine;
}

entity* create_simpel(std::string shader_name, entity_component* ec)
{
    entity* game_text = new entity(new shader(shader_name));
    game_text->add_component(ec);
}

entity* create_mesh(std::string shader_name, vec3f pos, int scale, std::vector<light>* lights, entity_component* ec)
{
    entity* e = new entity(new shader(shader_name), pos);
    e->add_component(ec);
    e->get_shader()->add_lights(lights);
    e->get_transform()->set_scale(scale);
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

    std::vector<entity*> entities, waterDraws, shadowDraws, shadowTmp;





    ///////

    water_fbo wfb;
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



    entity* game_text = new entity(new shader("text.glsl"));
    game_text->add_component(new text_component("3D Game Engine", "arial", 3.0f, vec2f(0.0f, 0.0f), 1.0f, true));


    entity* _particle = new entity(new shader("particle.glsl"), vec3f(40, 80, 40));
    _particle->add_component(new particle(hud_mesh, vec3f(0, 2, 0), 0.5f, 4.0, 50));
    _particle->get_transform()->set_scale(5);

    std::vector<light> lights;
    lights.push_back(light(vec3f(10000, 15000, -10000), vec3f(0.9, 0.9, 0.9))); //Sun



    entity* box_animation = create_mesh("animation.glsl", vec3f(40, 45, 40), 1, &lights, new animation_component("box", &shadow_mvp, &depth_map, "", ""));
    entity* monkey = create_mesh("basic.glsl", vec3f(30, 45, 40), 5, &lights, new mesh_component(new mesh("monkey"), &shadow_mvp, &depth_map, "default.jpg", ""));
    entity* monkey2 = create_mesh("basic_normal_mapping.glsl", vec3f(30, 65, 60), 5, &lights, new mesh_component(new mesh("monkey"), &shadow_mvp, &depth_map, "bricks.jpg", "bricks_normal.jpg"));



    mat4f* perspective = new mat4f();
    perspective->init_perspective(ToRadians(70.0f), Window->get_aspect(), 0.1f, 1000.0f);

    entity* Camera = new entity(NULL, vec3f(40, 40, 40));
    Camera->add_component(new camera_component(*perspective));
    Camera->add_component(new third_person(monkey->get_transform(), Window->get_center()));
    camera* cam = new camera(*perspective, Camera->get_transform());


    entity* terrain_mesh = new entity(new shader("testTerrain.glsl"));
    terrain_component* t = new terrain_component(&shadow_mvp, &depth_map, 0, 0, "terrain/empty.png", "terrain/grass.jpg", "terrain/flowers.jpg", "terrain/road.jpg", "terrain/blendmap.png");
    terrain_mesh->add_component(t);
    terrain_mesh->get_shader()->add_lights(&lights);

    entity* skybox = new entity(new shader("skybox.glsl"));
    skybox->add_component(new skybox_component(500));

    entity* health_bar = new entity(new shader("gui.glsl"), vec3f(150, Window->get_height() - 150, 0));
    health_bar->get_transform()->set_scale(0.3);
    health_bar->add_component(new gui_component("gui/healthBar.png", hud_mesh, Window->get_width(), Window->get_height()));



    entity* water = new entity(new shader("water.glsl"), vec3f(80, 40, 50), quaternion(), 500);
    water->add_component(new water_component("water/waterDUDV.png", "water/waterNormal.png", &wfb));

    entity* shadow = new entity(new shader("shadow.glsl"));
    shadow->add_component(new shadow_component(&shadowDraws, &depth_map, &shadow_mvp));

    shadowTmp.push_back(shadow);

    std::vector<entity*> phy_obj;
    phy_obj.push_back(monkey);
    phy_obj.push_back(monkey2);


    entities.push_back(Camera);
    entities.push_back(monkey);
    entities.push_back(box_animation);
    entities.push_back(terrain_mesh);
    entities.push_back(skybox);
    entities.push_back(water);
    entities.push_back(monkey2);

    entities.push_back(_particle);
    entities.push_back(health_bar);
    entities.push_back(game_text);


    waterDraws.push_back(terrain_mesh);
    waterDraws.push_back(monkey);
    waterDraws.push_back(monkey2);
    waterDraws.push_back(box_animation);
    waterDraws.push_back(skybox);

    shadowDraws.push_back(monkey);
    shadowDraws.push_back(monkey2);
    shadowDraws.push_back(box_animation);

    physics_obj* p_player = new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5), monkey->get_transform());

    std::vector<physics_obj*> physics_objs;
    physics_objs.push_back(new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5), monkey2->get_transform()));

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

            //update logic and input
            for (entity* e : entities)
            {
                e->process_input(Window->get_input(), m_frameTime);
                e->update(m_frameTime);
            }

            physicsEngine->update_terrain(t, phy_obj);
            physicsEngine->update_objs(p_player, physics_objs);

            /// day / night
            vec3f tmp = lights.at(0).get_color();
            tmp.set(tmp.get_x() + s, tmp.get_y() + s, tmp.get_z() + s);
            //lights.at(0).set_color(tmp);

            if (tmp.get_y() > 2) s = -0.001;
            if (tmp.get_y() < 0.2) s = 0.001;
            ///////////////////////

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
            Window->bind_as_render_targets();

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            post_p->get_shader()->use_shader();

            blur.set(0.0 / Window->WIN_WIDTH, 0.0);
            post_p->set_all_uni(*cam);
            post_p->render();
            ////////////////////////////////////

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
 */