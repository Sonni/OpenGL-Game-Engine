#include "animated_mesh.hpp"

animation_component::animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj, const std::string& tex_file_name, const std::string& normal_map) :
        m_tna_model(file_name),
        m_phy_obj(phy_obj)
{
    this->m_shadow_mvp = shadowMap;
    this->m_depth_map = depth_map;
    m_tex = new texture("diffuse.png");
    m_mesh = new mesh(file_name, m_tna_model.to_indexed_model(), true);
}

animation_component::~animation_component()
{
    delete m_tex;
    delete m_mesh;
}


void animation_component::init()
{
    get_shader()->add_uniform("model_tex");
    get_shader()->add_uniform("model");
    get_shader()->add_uniform("mvp");
    get_shader()->add_uniform("base_color");
    get_shader()->add_uniform("ambient_light");

    get_shader()->add_uniform("specular_intensity");
    get_shader()->add_uniform("specular_power");
    get_shader()->add_uniform("eye_pos");
    get_shader()->add_uniform("shadow_mvp");
    get_shader()->add_uniform("shadow_tex");

    get_shader()->set_light_loc();

    for (int i = 0; i < 50; i++)
    {
        get_shader()->add_uniform("joint_mat[" + std::to_string(i) + "]");
    }
}

void animation_component::set_all_uni(camera& cam)
{
    const mat4f mvp = cam.get_view_projection() * get_transform()->get_transformation();

    get_shader()->set_uniform_mat4f("mvp", mvp);
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_3f("base_color", vec3f(1, 1, 1));

    get_shader()->set_uniform_3f("ambient_light", vec3f(0.3, 0.3, 0.3));

    get_shader()->set_light();

    get_shader()->set_uniform_1f("specular_intensity", 1.0f);
    get_shader()->set_uniform_1f("specular_power", 8.0f);

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());
    get_shader()->set_uniform_mat4f("shadow_mvp", *m_shadow_mvp);

    m_tex->bind(0);
    get_shader()->set_uniform_1i("model_tex", 0);

    m_depth_map->bind(1, true);
    get_shader()->set_uniform_1i("shadow_tex", 1);

    set_joint_uniforms(m_tna_model.m_root);
}


void animation_component::set_joint_uniforms(joint *r) const
{
    get_shader()->set_uniform_mat4f("joint_mat[" + std::to_string(r->m_id) + "]", r->m_animated_transform);

    for (joint* j : r->m_children)
    {
        set_joint_uniforms(j);
    }
}

void animation_component::update(float delta, const camera &cam)
{
    if (m_is_moving)
        m_tna_model.update(delta);
}

void animation_component::render() const
{
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glDisable(GL_CULL_FACE);

    m_mesh->draw();

    glEnable(GL_CULL_FACE);
}

void animation_component::process_input(const input& input, float delta)
{
    m_is_moving = false;

    if(input.get_key_down(input.KEY_W))
        m_is_moving = true;
    if(input.get_key_down(input.KEY_S))
        m_is_moving = true;
    if(input.get_key_down(input.KEY_E))
        m_is_moving = true;
    if(input.get_key_down(input.KEY_Q))
        m_is_moving = true;

    if (!m_is_moving)
    {
        m_tna_model.set_time(0.25f);
        m_tna_model.update(delta);
    }
}


