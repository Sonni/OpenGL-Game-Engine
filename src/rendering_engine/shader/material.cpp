#include "material.hpp"
#include "../shader/shader.hpp"

void material::set_all_uniforms(shader* shader_p) {
    for(auto const& [key, val] : m_ints)
        shader_p->set_uniform_1i(key, val);

    for(auto const& [key, val] : m_floats)
        shader_p->set_uniform_1f(key, val);

    for(auto const& [key, val] : m_vec2f)
        shader_p->set_uniform_2f(key, val);

    for(auto const& [key, val] : m_vec3f)
        shader_p->set_uniform_3f(key, val);

    int counter = 0;

    if (m_diffuse_map) { shader_p->set_sampler_2d(m_DIFFUSE_NAME, m_diffuse_map, counter); counter++; }
    if (m_normal_map) { shader_p->set_sampler_2d(m_NORMAL_NAME, m_normal_map, counter); counter++; }
    if (m_disp_map) { shader_p->set_sampler_2d(m_DISP_NAME, m_disp_map, counter); counter++; }
    if (m_info_map) { shader_p->set_sampler_2d(m_INFO_NAME, m_info_map, counter); counter++; }
    for(auto const& [key, val] : m_textures)
    {
        shader_p->set_sampler_2d(key, const_cast<texture *>(&val), counter);
        counter++;
    }
}