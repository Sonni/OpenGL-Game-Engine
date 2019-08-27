#include "advanced_mesh.hpp"

advanced_mesh::advanced_mesh(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const vec3f& ambient, physics_obj* phy_obj, const std::string& tex_name, const std::string& normal_name, const std::string& disp_name, const std::string& specular_name) :
m_mesh(_mesh)
, m_s_ambientLight(ambient),
m_phy_obj(phy_obj)
{
    m_tex = new texture(tex_name);
    m_normal = new texture("normal/" + normal_name);
    m_disp_map = new texture("disp/" + disp_name);
    m_specular_map = new texture(specular_name);

    this->m_shadow_mvp = shadowMap;
    this->m_depth_map = depth_map;
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

    const mat4f mvp = cam.get_view_projection() * get_transform()->get_transformation();

    get_shader()->set_uniform_mat4f("mvp", mvp);
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_3f("base_color", vec3f(1, 1, 1));

    get_shader()->set_uniform_3f("ambient_light", m_s_ambientLight);

    get_shader()->set_light();

    get_shader()->set_uniform_1f("specular_intensity", 0.2f);
    get_shader()->set_uniform_1f("specular_power", 0.7f);

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());

    m_tex->bind(0);
    get_shader()->set_uniform_1i("sampler", 0);

    m_normal->bind(1);
    get_shader()->set_uniform_1i("normal_map", 1);

    m_disp_map->bind(2);
    get_shader()->set_uniform_1i("disp_tex", 2);

    m_specular_map->bind(3);
    get_shader()->set_uniform_1i("specu_tex", 3);

    get_shader()->set_uniform_mat4f("shadow_mvp", *m_shadow_mvp);

    m_depth_map->bind(4, true);
    get_shader()->set_uniform_1i("shadow_tex", 4);


}

void advanced_mesh::update(float delta, const camera &cam)
{
    if (m_phy_obj != nullptr)
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


void advanced_mesh::render() const
{
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        m_mesh->draw();

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
}