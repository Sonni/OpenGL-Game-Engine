#include "gui.hpp"

gui_component::gui_component(const std::string& file_name, mesh* Mesh, int win_width, int win_height) : m_win_width(win_width), m_win_height(win_height)
{
    m_use_texture_class = true;
    m_gui_tex = new texture(file_name);
    m_mesh = Mesh;
}

gui_component::gui_component(GLuint tex, mesh* Mesh, int win_width, int win_height) : m_win_width(win_width), m_win_height(win_height)
{
    m_use_texture_class = false;
    m_mesh = Mesh;
    m_texture_id = tex;
}

gui_component::~gui_component()
{
    delete m_gui_tex;
}

void gui_component::init()
{

    float scaleFactor;

    if (m_use_texture_class)
        scaleFactor = get_transform()->get_scale() * ((float) m_gui_tex->get_width() / m_win_width);
    else
        scaleFactor = get_transform()->get_scale() * ((float) 2048 / m_win_width);

    get_transform()->set_scale(scaleFactor);

    float x = (get_transform()->get_pos()->get_x() / m_win_width - 0.5f) * 2;
    float y = (get_transform()->get_pos()->get_y() / m_win_height - 0.5f) * 2;

    get_transform()->set_pos(vec3f(x, y, 0.0));

    get_shader()->add_uniform("model_tex");
    get_shader()->add_uniform("model");
}


void gui_component::set_all_uni(camera& cam)
{
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());

    if (m_use_texture_class)
    {
        m_gui_tex->bind(0);
        get_shader()->set_uniform_1i("model_tex", 0);
    }
    else
    {
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D, m_texture_id);
        get_shader()->set_uniform_1i("model_tex", 5);
    }
}

void gui_component::render() const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    m_mesh->draw();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}