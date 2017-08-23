#include "simpel_mesh.h"
#include "../../../shader/shader.h"

mesh_component::mesh_component(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const std::string& tex_file_name, const std::string& normal_map)
{
    this->_mesh = _mesh;
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;
    tex = new texture(tex_file_name);
    disp_map = new texture("disp/bricks_disp.jpg");
    if (normal_map.compare("") == 0)
    {
        has_normal_map = false;
    }
    else
    {
        this->normal_map = new texture(normal_map);
        has_normal_map = true;
    }
}

mesh_component::~mesh_component()
{
}

void mesh_component::init()
{
    get_shader()->add_uniform("model_tex");

    get_shader()->add_uniform("view_projection");
    get_shader()->add_uniform("model");
    get_shader()->add_uniform("inverted_view");

    get_shader()->add_uniform("plane");

    get_shader()->add_uniform("shadow_mvp");
    get_shader()->add_uniform("shadow_tex");

    if (has_normal_map)
    {
        get_shader()->add_uniform("normal_tex");
        get_shader()->add_uniform("disp_tex");
    }

    get_shader()->get_light_loc();

}

void mesh_component::set_all_uni(camera& cam)
{
    mat4f inverted_view = cam.get_view_matrix().invert();

    get_shader()->set_uniform_mat4f("view_projection", cam.get_view_projection());
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_mat4f("inverted_view", inverted_view);

    get_shader()->set_uniform_mat4f("shadow_mvp", *shadow_mvp);


    get_shader()->set_uniform_4f("plane", cam.get_cutting_plane());

    tex->bind(0);
    get_shader()->set_uniform_1i("model_tex", 0);

    depth_map->bind(1, true);
    get_shader()->set_uniform_1i("shadow_tex", 1);



    if (has_normal_map)
    {
        normal_map->bind(2);
        get_shader()->set_uniform_1i("normal_tex", 2);

        disp_map->bind(3);
        get_shader()->set_uniform_1i("disp_tex", 3);
    }

    get_shader()->set_light();
}

void mesh_component::render() const
{
    _mesh->draw();
}