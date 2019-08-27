#ifndef INC_3D_ENGINE_GUI_HPP
#define INC_3D_ENGINE_GUI_HPP

#include "../../../rendering_engine/shader/shader.hpp"


class gui_component : public entity_component
{
public:
    gui_component(const std::string& file_name, mesh* Mesh, int win_width, int win_height);
    gui_component(GLuint tex, mesh* Mesh, int win_width, int win_height);

    ~gui_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void render() const;

private:
    mesh* m_mesh;
    texture* m_gui_tex;
    int m_win_width, m_win_height;

    GLuint m_texture_id;
    bool m_use_texture_class;
};


#endif //INC_3D_ENGINE_GUI_HPP
