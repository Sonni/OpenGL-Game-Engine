#include "advanced_mesh.h"

advanced_mesh::advanced_mesh(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const vec3f& ambient, physics_obj* phy_obj, const std::string& tex_name, const std::string& normal_name, const std::string& disp_name, const std::string& specular_name) :
_mesh(_mesh)
, s_ambientLight(ambient),
phy_obj(phy_obj)
{
    tex = new texture(tex_name);
    normal = new texture("normal/" + normal_name);
    disp_map = new texture("disp/" + disp_name);
    specular_map = new texture("specular/" + specular_name);

    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;
}

void advanced_mesh::init()
{
    get_shader()->add_uniform("model");
    get_shader()->add_uniform("mvp");
    get_shader()->add_uniform("base_color");
    get_shader()->add_uniform("ambient_light");

    get_shader()->add_uniform("specular_intensity");
    get_shader()->add_uniform("specular_power");
    get_shader()->add_uniform("eye_pos");

    get_shader()->add_uniform("sampler");
    get_shader()->add_uniform("normal_map");
    get_shader()->add_uniform("disp_tex");
    get_shader()->add_uniform("specu_tex");

    get_shader()->add_uniform("shadow_mvp");
    get_shader()->add_uniform("shadow_tex");

    get_shader()->set_light_loc();
}

void advanced_mesh::set_all_uni(camera& cam)
{

    mat4f mvp = cam.get_view_projection() * get_transform()->get_transformation();

    get_shader()->set_uniform_mat4f("mvp", mvp);
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_3f("base_color", vec3f(1, 1, 1));

    get_shader()->set_uniform_3f("ambient_light", s_ambientLight);

    get_shader()->set_light();

    get_shader()->set_uniform_1f("specular_intensity", 1.0f);
    get_shader()->set_uniform_1f("specular_power", 8.0f);

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());

    tex->bind(0);
    get_shader()->set_uniform_1i("sampler", 0);

    normal->bind(1);
    get_shader()->set_uniform_1i("normal_map", 1);

    disp_map->bind(2);
    get_shader()->set_uniform_1i("disp_tex", 2);

    get_shader()->set_uniform_mat4f("shadow_mvp", *shadow_mvp);

    depth_map->bind(3, true);
    get_shader()->set_uniform_1i("shadow_tex", 3);

    specular_map->bind(4);
    get_shader()->set_uniform_1i("specu_tex", 4);
}

void advanced_mesh::update(float delta, const camera &cam)
{
    if (phy_obj != nullptr)
    {
        phy_obj->get_collider()->set_pos(*get_transform()->get_pos());

        if (cam.get_frustum().collider_in_frustum(phy_obj->get_collider())) {
            draw = true;
        }
        else {
            draw = false;
        }
    }
}

void advanced_mesh::render() const
{
    if (draw)
    {
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _mesh->draw();

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
    }
}