#include "core_engine.hpp"
#include "component/water/water.hpp"
#include "component/shadow/shadow.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../rendering_engine/stb_image_write.h"
#include "component/light_scatter.hpp"
#include "../utils/shared.hpp"
#include <thread>
//#include "../network/client/old/TCPClient.h"


int save_screenshot(const char *filename)
{
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    int x = viewport[0];
    int y = viewport[1];
    int width = viewport[2];
    int height = viewport[3];

    char *data = (char*) malloc((size_t) (width * height * 3)); // 3 components (R, G, B)

    if (!data)
        return 0;

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

    int saved = stbi_write_png(filename, width, height, 3, data, 0);

    free(data);

    return saved;
}

core_engine::core_engine(window* Window, rendering_engine* renderingEngine, physics_engine* physicsEngine)
{
    m_window = Window;
    m_renderingEngine = renderingEngine;
    m_physicsEngine = physicsEngine;
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
    m_entities.push_back(Entity);
}

void core_engine::add_physics_obj(entity *Entity)
{
    m_phy_obj.push_back(Entity);
}

void core_engine::add_water_and_shadow(entity *Entity)
{
    m_waterDraws.push_back(Entity);
    m_shadowDraws.push_back(Entity);
}

void core_engine::add_water_reflect(entity *Entity)
{
    m_waterDraws.push_back(Entity);
}

void core_engine::add_scatter(entity *Entity)
{
    m_scatters.push_back(Entity);
}

void core_engine::set_camera(camera *cam, entity *Camera)
{
    m_cam = cam;
    m_Camera = Camera;
}


void core_engine::set_frustum(frustum *f)
{
    m_Frustum = f;
}

extern bool g_draw_tex;

bool core_engine::run()
{


    ///////
    std::vector<entity*> shadowTmp;
    std::vector<terrain_component*> terrains;

    water_fbo wfb;
    mat4f shadow_mvp;
    texture depth_map(1024, 1024, true);
    texture post_processing(m_window->WIN_WIDTH, m_window->WIN_HEIGHT, 0, GL_TEXTURE_2D, GL_LINEAR, GL_RGBA, GL_RGBA, true, GL_COLOR_ATTACHMENT0);
    mesh* hud_mesh = m_renderingEngine->get_hud_mesh();

    water_fbo light_scatter_fbo;
    entity* light_scatter = new entity(new shader("light_scatter.glsl"));
    light_scatter->add_component(new light_scatter_component(&light_scatter_fbo));

    //////


    physics_obj* mk2 = new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5));

    vec2f blur;

    water_component* water_comp = new water_component(400, 400, "water/waterDUDV.png", "water/waterNormal.png", &wfb, m_cam, &m_waterDraws);

    entity* post_p = create_mesh("gaussian_blur.glsl", vec3f(), 1, new blur_component(&post_processing, hud_mesh, &blur));
    entity* shadow = create_mesh("shadow.glsl", vec3f(), 1, new shadow_component(&m_shadowDraws, &depth_map, &shadow_mvp));



    entity* water = create_mesh("water/water_waves.glsl", vec3f(0, -2000, 0), 1, water_comp);
    add_entity(water);



    shadowTmp.push_back(shadow);



    std::vector<physics_obj*> physics_objs;
    physics_objs.push_back(mk2);
    //physics_objs.push_back(new physics_obj(new sphere(vec3f(0.0f, 0.0f, 0.0f), 5), monkey2->get_transform()));
    //physics_objs.push_back(new physics_obj(new aabb(vec3f(0, 0, 0), vec3f(s, s, s)), monkey2->get_transform()));


     /*if (SDLNet_Init() == -1)
     {
         printf("SDLNet_Init: %s\n", SDLNet_GetError());
         exit(2);
     }

     TCPClient tcpClient("127.0.0.1", 3400);
     std::thread t1(&TCPClient::start, &tcpClient);*/

    stbi_flip_vertically_on_write(1);

    bool button_down = false;

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
            m_window->update();

            if(m_window->is_closed_requested())
            {
                m_isRunning = false;
            }

            //update logic and input
            for (entity* e : m_entities)
            {
                e->process_input(m_window->get_input(), m_updateTime);
                e->update(m_updateTime, *m_cam);
            }

            m_Frustum->update_cam(*m_cam->get_transform()->get_pos(), m_cam->get_transform()->get_rot()->get_back(), vec3f(0,1,0));

            m_physicsEngine->update_terrain(terrains, m_phy_obj);
            //physicsEngine->update_objs(monkey, p_player, physics_objs);
            m_physicsEngine->update_gravity(m_phy_obj, m_updateTime);

            if (m_window->get_input().get_key_down(m_window->get_input().KEY_7) && !button_down)
            {
                save_screenshot("test.png");
                button_down = true;
            }
            if (m_window->get_input().get_key_up(m_window->get_input().KEY_7)) button_down = false;

            unprocessedTime -= m_updateTime;
            updates++;
        }

        if (secondCounter > 1000)
        {
            std::cout << "FPS: " << frames << " Updates: " << updates << std::endl;

            frames = updates = 0;
            secondCounter = 0.0;

            std::string pos = "<M> " + std::to_string(m_cam->get_transform()->get_pos()->get_x()) + " " + std::to_string(m_cam->get_transform()->get_pos()->get_y()) + " " + std::to_string(m_cam->get_transform()->get_pos()->get_z());

            //tcpClient.send(pos);
        }


        //update graphics
        if(frameCounter > m_frameTime)
        {
            post_processing.bind_as_render_target();
            /// Render shadows
            shadow->set_all_uni(*m_cam);
            shadow->render();
            ///////////////////

            /// Render water
            water_comp->render_reflect_refract();
            /////////////////////////////////////////////////////////////////////////////


            /// Light scatter
            light_scatter_fbo.bind_reflect_fbo();

            // TODO Change to lower resolution, aka viewport
            glDisable(GL_TEXTURE_2D);
            g_draw_tex = false;
            m_renderingEngine->render(m_cam, m_scatters);
            g_draw_tex = true;
            glEnable(GL_TEXTURE_2D);


            ////////////////


            /// Render scene
            post_processing.bind_as_render_target();
            m_renderingEngine->render(m_cam, m_entities);

            ////////////

            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);

            glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
            glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

            light_scatter->get_shader()->use_shader();
            light_scatter->set_all_uni(*m_cam);
            light_scatter->render();


            glDisable(GL_BLEND);

            ///Render post processed tex to window
            m_window->bind_as_render_targets();

            glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glEnable(GL_DEPTH_TEST);

            post_p->get_shader()->use_shader();

            blur.set(0.0 / m_window->WIN_WIDTH, 0.0);
            post_p->set_all_uni(*m_cam);
            post_p->render();

            ////////////////////////////////////

            m_window->swap_buffers();
            frames++;
            frameCounter -= m_frameTime;
        }
        else
        {
            SDL_Delay(15);
        }
    }

    //Clean up
    for (int i = 0; i < m_entities.size(); i++)
    {
        delete m_entities.at(i);
    }
    delete shadow;
    delete hud_mesh;
    delete m_cam;
    //if (FL) delete FL;
    //if (FM) delete FM;
    //if (TP) delete TP;

    return true;
}