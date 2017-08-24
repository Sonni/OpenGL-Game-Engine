#include "animated_mesh.h"

animation_component::animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj, const std::string& tex_file_name, const std::string& normal_map) :
        _mesh(file_name),
        tna_model(file_name),
        phy_obj(phy_obj)
{
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;
    tex = new texture("default.png");
}

animation_component::~animation_component()
{
    delete tex;
}

void animation_component::init()
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

    for (int i = 0; i < tna_model.m_bones.size(); i++)
    {
        get_shader()->get_uniform("skinning_mat[" + std::to_string(i) + "]");
    }


    get_shader()->set_light_loc();

}

void animation_component::set_all_uni(camera& cam)
{
    mat4f mvp = cam.get_view_projection() * get_transform()->get_transformation();

    get_shader()->set_uniform_mat4f("mvp", mvp);
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_3f("base_color", vec3f(1, 1, 1));
    get_shader()->set_uniform_mat4f("shadow_mvp", *shadow_mvp);


    get_shader()->set_uniform_3f("ambient_light", vec3f(0.3, 0.3, 0.3));

    get_shader()->set_light();

    get_shader()->set_uniform_1f("specular_intensity", 1.0f);
    get_shader()->set_uniform_1f("specular_power", 8.0f);

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());

    tex->bind(0);
    get_shader()->set_uniform_1i("sampler", 0);

    depth_map->bind(1, true);
    get_shader()->set_uniform_1i("shadow_tex", 1);

}

float counter = 0;
float animationCounter = 0;
float leftOver = 0;
void animation_component::update(float delta, const camera &cam)
{
    if (phy_obj != NULL)
    {
        phy_obj->get_collider()->set_pos(*get_transform()->get_pos());

        if (cam.get_frustum().collider_in_frustum(phy_obj->get_collider())) {
            draw = true;
        }
        else {
            draw = false;
        }
    }

    counter += 500.0f * delta;

    if(counter > 10)
    {
        animationCounter += 1;
        leftOver += fmodf(counter, 10);

        if(leftOver > 10)
        {
            animationCounter++;
            leftOver = 0;
        }

        if(animationCounter >= 20 * 2 + tna_model.m_animation[0].m_time.size())
        {
            animationCounter = 0;
        }
        else
        {
            tna_model.CalcBonesWorldMatrix(animationCounter);
            counter = 0;
        }
    }

}

void animation_component::render() const
{
    if (draw)
    {
        std::vector<mat4f> boneMats;

        for (int i = 0; i < tna_model.m_bones.size(); i++) {
            std::ostringstream name;
            name << "skinning_mat[" << i << "]";

            get_shader()->set_uniform_mat4f(name.str(), tna_model.m_bones[i].m_skinningMatrix);
        }

        _mesh.draw();
    }
}