#include "components.h"
#include "../../rendering_engine/stb_image.h"


void camera_component::set_parent(entity *parent)
{
    entity_component::set_parent(parent);

}

/////////////////// GUI ////////////////////
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

/////////////////// SKYBOX ////////////////////
skybox_component::skybox_component()
{
    std::vector<std::string> tex_names(m_file_names, m_file_names + sizeof(m_file_names) / sizeof(m_file_names[0]));
    m_tex_loc = load_cube_map(tex_names);

    aabb box(vec3f(-200, -200, -200), vec3f(200, 200, 200));
    m_cube = simple_cube(box, "skybox");
}

skybox_component::~skybox_component()  { }

void skybox_component::init()
{
    get_shader()->add_uniform("view_projection");
    get_shader()->add_uniform("cube_map");
}

void skybox_component::set_all_uni(camera& cam)
{
    mat4f fixed_view = cam.get_view_matrix();
    fixed_view[3][0] = 0;
    fixed_view[3][1] = 0;
    fixed_view[3][2] = 0;

    mat4f scale;
    scale.init_scale(vec3f(get_transform()->get_scale(), get_transform()->get_scale(), get_transform()->get_scale()));

    mat4f fixed_view_projection = cam.get_projection_matrix() * fixed_view * scale;

    get_shader()->set_uniform_mat4f("view_projection", fixed_view_projection);

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_tex_loc);
    get_shader()->set_uniform_1i("cube_map", 0);
}

void skybox_component::update(float delta, const camera &cam)
{

}

void skybox_component::render() const
{
    glDepthMask(GL_FALSE);
    m_cube.render();
    glDepthMask(GL_TRUE);

}

GLuint skybox_component::load_cube_map(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        std::string file_name = "../res/textures/skybox/skybox2/" + faces[i] + ".png";
        unsigned char *data = stbi_load(file_name.c_str(), &width, &height, &nrChannels, 3);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << file_name << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

