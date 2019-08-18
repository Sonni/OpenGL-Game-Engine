#include "simpel_mesh.h"
#include "../../../shader/shader.h"

mesh_component::mesh_component(mesh* _mesh, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj, const vec3f& ambient, const std::string& tex_file_name) :
    m_ambient(ambient),
    m_phy_obj(phy_obj)
{
    this->m_mesh = _mesh;
    this->m_shadow_mvp = shadowMap;
    this->m_depth_map = depth_map;
    m_tex = new texture(tex_file_name);
}

mesh_component::~mesh_component()  { }

void mesh_component::init()
{
    get_shader()->add_uniform("model");
    get_shader()->add_uniform("mvp");
    get_shader()->add_uniform("base_color");
    get_shader()->add_uniform("ambient_light");

    get_shader()->add_uniform("specular_intensity");
    get_shader()->add_uniform("specular_power");
    get_shader()->add_uniform("eye_pos");

    get_shader()->add_uniform("sampler");

    get_shader()->add_uniform("shadow_mvp");
    get_shader()->add_uniform("shadow_tex");

    get_shader()->set_light_loc();
}

void mesh_component::set_all_uni(camera& cam)
{
    mat4f mvp = cam.get_view_projection() * get_transform()->get_transformation();

    get_shader()->set_uniform_mat4f("mvp", mvp);
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_3f("base_color", vec3f(1, 1, 1));
    get_shader()->set_uniform_mat4f("shadow_mvp", *m_shadow_mvp);


    get_shader()->set_uniform_3f("ambient_light", m_ambient);

    get_shader()->set_light();

    get_shader()->set_uniform_1f("specular_intensity", 1.0f);
    get_shader()->set_uniform_1f("specular_power", 8.0f);

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());

    m_tex->bind(0);
    get_shader()->set_uniform_1i("sampler", 0);

    m_depth_map->bind(1, true);
    get_shader()->set_uniform_1i("shadow_tex", 1);
}

void mesh_component::update(float delta, const camera &cam)
{
    if (m_phy_obj != NULL)
    {
        m_phy_obj->get_collider()->set_pos(*get_transform()->get_pos());

        if (cam.get_frustum().collider_in_frustum(m_phy_obj->get_collider())) {
            m_draw = true;
        }
        else {
            m_draw = false;
        }
    }
}

void mesh_component::render() const
{
    if (m_draw)
    {
        m_mesh->draw();
    }
}