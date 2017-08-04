#include "rendering_engine.h"
#include "../shader/shader.h"


rendering_engine::rendering_engine()
{

}

void rendering_engine::render(camera* cam, std::vector<entity*> e) {


    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    for (int i = 0; i < e.size(); i++) {
        if (e.at(i)->get_shader() != NULL)
        {
            e.at(i)->get_shader()->use_shader();
            e.at(i)->set_all_uni(*cam);


            e.at(i)->render();
        }
    }



}