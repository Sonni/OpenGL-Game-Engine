#ifndef INC_3D_ENGINE_LIGHT_SCATTER_HPP
#define INC_3D_ENGINE_LIGHT_SCATTER_HPP


#include "../../rendering_engine/shader/shader.hpp"
#include "water/water_fbo.h"
#include "../core_engine.hpp"

#include <GL/glew.h>


extern core_engine *g_core_engine;

class light_scatter_component : public entity_component {
public:
   /* float m_exposure = 0.0034f;
    float m_decay = 1.0f;
    float m_density = 0.84f;
    float m_weight = 5.65f;*/

   float m_exposure = 0.0008f;
    float m_decay = 1.0f;
    float m_density = 0.84f;
    float m_weight = 7.65;

    water_fbo* m_fbo;
    mesh* m_mesh;

    light_scatter_component(water_fbo* fbo) : m_fbo(fbo) {
        indexed_model hud_square;
        {
            hud_square.add_vertex(1.0f, -1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(1.0f, 0.0f));
            hud_square.add_vertex(1.0f, 1.0f, 0.0f);   hud_square.add_tex_coord(vec2f(1.0f, 1.0f));
            hud_square.add_vertex(-1.0f, -1.0f, 0.0f); hud_square.add_tex_coord(vec2f(0.0f, 0.0f));
            hud_square.add_vertex(-1.0f, 1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(0.0f, 1.0f));
            hud_square.add_face(0, 1, 2); hud_square.add_face(2, 1, 3);
        }
        m_mesh = new mesh("light_scatter", hud_square.finalize());
    }

    virtual void init()
    {
        get_shader()->use_shader();

        get_shader()->add_uniform("model");
        get_shader()->add_uniform("exposure");
        get_shader()->add_uniform("decay");
        get_shader()->add_uniform("density");
        get_shader()->add_uniform("weight");
        get_shader()->add_uniform("lightPositionOnScreen");
        get_shader()->add_uniform("myTexture");

    }

    virtual void set_all_uni(camera& cam)
    {
        get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());

        get_shader()->set_uniform_1f("exposure", m_exposure);
        get_shader()->set_uniform_1f("decay", m_decay);
        get_shader()->set_uniform_1f("density", m_density);
        get_shader()->set_uniform_1f("weight", m_weight);


        get_shader()->set_uniform_2f("lightPositionOnScreen", world_to_screen(cam.get_view_projection(), vec3f(10000, 3000, -10000)));


        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_fbo->get_reflect_tex());
        glUniform1i(get_shader()->get_uniform("myTexture"), 0);
    }


    virtual void render() const
    {
        m_mesh->draw();
    }

    virtual void update(float delta, const camera &cam) { }

    vec2f world_to_screen(mat4f mvp_matrix, vec3f sun_pos)
    {
        // multiplying world coordinates * projection matrix = clip_coords
        vec4f clip_coords = mvp_matrix.transform(vec4f(sun_pos.get_x(), sun_pos.get_y(), sun_pos.get_z(), 1));

        if (clip_coords[3] < 0.1f) return vec2f(10, 10);

        //perspective division
        vec3f normalized_device_coords(vec3f(clip_coords.get_x(), clip_coords.get_y(), clip_coords.get_z()) / clip_coords[3]);

        //viewport tranform to screenCooords
        vec3f sun_screen_coords;

        sun_screen_coords[0] =  ((float) g_core_engine->get_window()->get_width() / 2 * normalized_device_coords[0]) + (normalized_device_coords[0] + (float) g_core_engine->get_window()->get_width() / 2);
        sun_screen_coords[1] =  ((float) g_core_engine->get_window()->get_height() / 2 * normalized_device_coords[1]) + (normalized_device_coords[1] + (float) g_core_engine->get_window()->get_height() / 2);

        return vec2f((float) (sun_screen_coords[0] * 2) / (float) 824 - 1.0, (float) (sun_screen_coords[1] * 2) / (float) 600 - 1.0);
    }

private:
};


#endif //INC_3D_ENGINE_LIGHT_SCATTER_HPP
