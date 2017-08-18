#include "shadow.h"
#include "../../io/window.h"

shadow_component::shadow_component(std::vector<entity*>* entities, texture* depth_map, mat4f* shadow_mvp, int shadowMapSizeAsPowerOf2, float shadowArea):
    half_shadow_area(shadowArea / 2.0f),
    shadow_map_power_2(shadowMapSizeAsPowerOf2),
    orto_projection(mat4f().init_orthographic(-half_shadow_area, half_shadow_area, -half_shadow_area, half_shadow_area, -half_shadow_area, half_shadow_area))
{
    this->entities = entities;
    this->depth_map = depth_map;
    this->shadow_mvp = shadow_mvp;

    BIAS_MATRIX = mat4f().init_scale(vec3f(0.5, 0.5, 0.5)) * mat4f().init_translation(vec3f(1.0, 1.0, 1.0));
}

void shadow_component::init()
{
    mvp_loc = get_shader()->get_uni_location("mvp");
}

void shadow_component::set_all_uni(camera& cam)
{
    transform shadowCameraTransform = get_shadow_transform(*cam.get_transform());

    mat4f conj_rot = shadowCameraTransform.get_rot()->negate().to_rotation_matrix();
    mat4f neg_translate;
    neg_translate.init_translation(*shadowCameraTransform.get_pos() * -1);

    light_view_projection =  orto_projection * conj_rot * neg_translate;
    *shadow_mvp = BIAS_MATRIX * light_view_projection;
}

void shadow_component::render() const
{
    depth_map->bind_as_render_target();

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    get_shader()->use_shader();

    for (int i = 0; i < entities->size(); i++)
    {
        mat4f mvp_matrix = light_view_projection * entities->at(i)->get_transform()->get_transformation();
        glUniformMatrix4fv(mvp_loc, 1, GL_FALSE, &mvp_matrix[0][0]);


        entities->at(i)->render();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    depth_map->unbind_render_target(window::WIN_WIDTH, window::WIN_HEIGHT);
}

transform shadow_component::get_shadow_transform(const transform &cam_transform) const
{
    vec3f resultPos = cam_transform.get_pos() + cam_transform.get_rot().get_forward() * half_shadow_area;
    quaternion resultRot(vec3f(1,0,0), ToRadians(-45));

    float worldTexelSize = (half_shadow_area*2)/((float)(1 << shadow_map_power_2));

    vec3f light_cam_pos = resultPos.rotate(resultRot.negate());

    light_cam_pos.set_x(worldTexelSize * floor(light_cam_pos.get_x() / worldTexelSize));
    light_cam_pos.set_y(worldTexelSize * floor(light_cam_pos.get_y() / worldTexelSize));

    resultPos = light_cam_pos.rotate(resultRot);

    transform result(resultPos, resultRot);

    return result;
}