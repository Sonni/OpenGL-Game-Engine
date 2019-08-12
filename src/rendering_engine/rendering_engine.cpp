#include "rendering_engine.h"
#include "../shader/shader.h"


rendering_engine::rendering_engine()
{
    indexed_model hud_square;
    {
        hud_square.add_vertex(1.0f, -1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(1.0f, 1.0f));
        hud_square.add_vertex(1.0f, 1.0f, 0.0f);   hud_square.add_tex_coord(vec2f(1.0f, 0.0f));
        hud_square.add_vertex(-1.0f, -1.0f, 0.0f); hud_square.add_tex_coord(vec2f(0.0f, 1.0f));
        hud_square.add_vertex(-1.0f, 1.0f, 0.0f);  hud_square.add_tex_coord(vec2f(0.0f, 0.0f));
        hud_square.add_face(0, 1, 2); hud_square.add_face(2, 1, 3);
    }
    hud_mesh = new mesh("squre", hud_square.finalize());
}

void rendering_engine::render(camera* cam, std::vector<entity*> e)
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (int i = 0; i < e.size(); i++)
    {
        if (e.at(i)->get_shader() != NULL)
        {
            e.at(i)->get_shader()->use_shader();
            e.at(i)->set_all_uni(*cam);

            e.at(i)->render();
        }
    }
}