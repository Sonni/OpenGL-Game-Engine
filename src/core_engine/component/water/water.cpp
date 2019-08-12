#include "water.h"

water_component::water_component(const std::string& dudv, const std::string& normalMap, water_fbo* wfb)
{
    dudv_tex = new texture(dudv);
    normal_tex = new texture(normalMap);
    this->wfb = wfb;
    m_mesh = new mesh("plane");
}

void water_component::init()
{
    model_loc = get_shader()->get_uni_location("model");
    mvp_loc = get_shader()->get_uni_location("mvp");


    dudv_loc = get_shader()->get_uni_location("dudv_tex");
    normal_loc = get_shader()->get_uni_location("normal_tex");

    move_factor_loc = get_shader()->get_uni_location("move_factor");

    refrac_loc = get_shader()->get_uni_location("refraction_tex");
    reflec_loc = get_shader()->get_uni_location("reflection_tex");
    depth_loc = get_shader()->get_uni_location("depth_tex");
    cam_pos_loc = get_shader()->get_uni_location("cam_pos");

    vec3f pos = *get_transform()->get_pos();
    float half_len = get_transform()->get_scale()/2;

    float y = pos.get_y();
    float x = pos.get_x() + half_len;
    float z = pos.get_z() + half_len;
    vec3f center(x, y, z);

    float radius = (float) sqrt(pow(half_len, 2) * 2);

    _sphere = new sphere(center, radius);

}


void water_component::set_all_uni(camera& cam)
{
    mat4f mvp_matrix = cam.get_view_projection() * get_transform()->get_transformation();

    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &get_transform()->get_transformation()[0][0]);
    glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &mvp_matrix[0][0]);

    glUniform3f(cam_pos_loc, cam.get_transform()->get_pos()->get_x(), cam.get_transform()->get_pos()->get_y(), cam.get_transform()->get_pos()->get_z());

    dudv_tex->bind(0);
    glUniform1i(dudv_loc, 0);

    normal_tex->bind(1);
    glUniform1i(normal_loc, 1);

    glActiveTexture(GL_TEXTURE0 + 2);
    glBindTexture(GL_TEXTURE_2D, wfb->get_reflect_tex());
    glUniform1i(reflec_loc, 2);

    glActiveTexture(GL_TEXTURE0 + 3);
    glBindTexture(GL_TEXTURE_2D, wfb->get_refract_tex());
    glUniform1i(refrac_loc, 3);

    glActiveTexture(GL_TEXTURE0 + 4);
    glBindTexture(GL_TEXTURE_2D, wfb->get_refract_depth_tex());
    glUniform1i(depth_loc, 4);

    moveFactor += 0.0001f ;

    moveFactor = fmodf(moveFactor, 1.0f);
    glUniform1f(move_factor_loc, moveFactor);

}

void water_component::update(float delta, const camera &cam)
{
    if (cam.get_frustum().sphere_in_frustum(*_sphere))
    {
        draw = true;
    }
    else
    {
        draw = false;
    }
}

void water_component::render() const
{
    if (draw)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        m_mesh->draw();
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
    }
}