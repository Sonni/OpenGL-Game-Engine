#ifndef INC_3D_ENGINE_CHAT_BOX_HPP
#define INC_3D_ENGINE_CHAT_BOX_HPP

#include "../../../rendering_engine/shader/shader.hpp"
#include "../../io/window.hpp"


class chat_box_component : public entity_component {
public:
    chat_box_component(int width, int height, window* Window) : m_width(width), m_height(height), m_window(Window)
    {

    }



    virtual void render() const
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        m_hud_mesh->draw();

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
    }

    virtual void init()
    {
        float real_width = (float) (m_width * 2) / (float) m_window->get_width();
        float real_height = (float) (m_height * 2) / (float) m_window->get_height();
        float x = (float) (get_transform()->get_pos()->get_x() * 2) / (float) m_window->get_width() - 1.0;
        float y = (float) (get_transform()->get_pos()->get_y() * 2) / (float) m_window->get_height() - 1.0;

        std::cout <<  x  << std::endl;

        //float x = get_transform()->get_pos()
        indexed_model hud_square;
        {
            hud_square.add_vertex(x, y, 0.0f);  hud_square.add_tex_coord(vec2f(1.0f, 1.0f));
            hud_square.add_vertex(x, y+real_height, 0.0f);   hud_square.add_tex_coord(vec2f(1.0f, 0.0f));
            hud_square.add_vertex(x+real_width, y+real_height, 0.0f); hud_square.add_tex_coord(vec2f(0.0f, 1.0f));
            hud_square.add_vertex(x+real_width, y, 0.0f);  hud_square.add_tex_coord(vec2f(0.0f, 0.0f));
            hud_square.add_face(0, 1, 2); hud_square.add_face(2, 0, 3);
        }
        m_hud_mesh = new mesh("chat_box", hud_square.finalize());

        get_shader()->add_uniform("model");
        get_shader()->add_uniform("color");
    }

    virtual void set_all_uni(camera& cam)
    {
        get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());

        get_shader()->set_uniform_4f("color", vec4f(0.0, 0.0, 0.5, 0.5));
    }

private:
    mesh* m_hud_mesh;
    int m_width, m_height;
    window* m_window;
};


#endif //INC_3D_ENGINE_CHAT_BOX_HPP
