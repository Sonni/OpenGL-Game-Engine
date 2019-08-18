#include "shadow.h"
#include "../../io/window.h"

shadow_component::shadow_component(std::vector<entity*>* entities, texture* depth_map, mat4f* shadow_mvp, int shadowMapSizeAsPowerOf2, float shadowArea):
        m_half_shadow_area(shadowArea / 2.0f),
        m_shadow_map_power_2(shadowMapSizeAsPowerOf2),
        m_orto_projection(mat4f().init_orthographic(-m_half_shadow_area, m_half_shadow_area, -m_half_shadow_area, m_half_shadow_area, -m_half_shadow_area, m_half_shadow_area))
{
    m_entities = entities;
    m_depth_map = depth_map;
    m_shadow_mvp = shadow_mvp;

    m_BIAS_MATRIX = mat4f().init_scale(vec3f(0.5, 0.5, 0.5)) * mat4f().init_translation(vec3f(1.0, 1.0, 1.0));
}

void shadow_component::init()
{
    m_mvp_loc = get_shader()->get_uni_location("mvp");
}

void shadow_component::set_all_uni(camera& cam)
{
    transform shadowCameraTransform = get_shadow_transform(*cam.get_transform());

    mat4f conj_rot = shadowCameraTransform.get_rot()->negate().to_rotation_matrix();
    mat4f neg_translate;
    neg_translate.init_translation(*shadowCameraTransform.get_pos() * -1);

    m_light_view_projection = m_orto_projection * conj_rot * neg_translate;
    *m_shadow_mvp = m_BIAS_MATRIX * m_light_view_projection;
}

void shadow_component::render() const
{
    m_depth_map->bind_as_render_target();

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    get_shader()->use_shader();

    for (int i = 0; i < m_entities->size(); i++)
    {
        mat4f mvp_matrix = m_light_view_projection * m_entities->at(i)->get_transform()->get_transformation();
        glUniformMatrix4fv(m_mvp_loc, 1, GL_FALSE, &mvp_matrix[0][0]);

        m_entities->at(i)->render();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_depth_map->unbind_render_target(window::WIN_WIDTH, window::WIN_HEIGHT);
}

transform shadow_component::get_shadow_transform(const transform &cam_transform) const
{
    vec3f resultPos = cam_transform.get_pos() + cam_transform.get_rot().get_forward() * m_half_shadow_area;
    quaternion resultRot(vec3f(1,0,0), ToRadians(-45));

    float worldTexelSize = (m_half_shadow_area*2)/((float)(1 << m_shadow_map_power_2));

    vec3f light_cam_pos = resultPos.rotate(resultRot.negate());

    light_cam_pos.set_x(worldTexelSize * floor(light_cam_pos.get_x() / worldTexelSize));
    light_cam_pos.set_y(worldTexelSize * floor(light_cam_pos.get_y() / worldTexelSize));

    resultPos = light_cam_pos.rotate(resultRot);

    transform result(resultPos, resultRot);

    return result;
}