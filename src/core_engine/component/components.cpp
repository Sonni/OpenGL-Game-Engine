#include "components.hpp"
#include "../../rendering_engine/stb_image.h"


void camera_component::set_parent(entity *parent)
{
    entity_component::set_parent(parent);

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

