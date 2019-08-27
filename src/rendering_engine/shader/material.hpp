#ifndef INC_3D_ENGINE_MATERIAL_HPP
#define INC_3D_ENGINE_MATERIAL_HPP

#include <map>
#include "../texture.hpp"
#include "../../core_engine/math/vector.hpp"

class shader;

class material {
public:
    material(const std::string& diffuse_map = "", const std::string& normal_map = "", const std::string& disp_map = "", const std::string& info_map = "")
    {
        if (diffuse_map.empty()) m_diffuse_map = nullptr;
        else m_diffuse_map = new texture(diffuse_map);

        if (normal_map.empty()) m_normal_map = nullptr;
        else m_normal_map = new texture(normal_map);

        if (disp_map.empty()) m_disp_map = nullptr;
        else m_disp_map = new texture(disp_map);

        if (info_map.empty()) m_info_map = nullptr;
        else m_info_map = new texture(info_map);
    }

    ~material()
    {
        if (m_diffuse_map) delete m_diffuse_map;
        if (m_normal_map) delete m_normal_map;
        if (m_disp_map) delete m_disp_map;
        if (m_info_map) delete m_info_map;
    }


    inline void set_int(const std::string& mat_name, int value) { m_ints[mat_name] = value; }
    inline void set_float(const std::string& mat_name, float value) { m_floats[mat_name] = value; }
    inline void set_vec2f(const std::string& mat_name, const vec2f& value) { m_vec2f[mat_name] = value; }
    inline void set_vec3f(const std::string& mat_name, const vec3f& value) { m_vec3f[mat_name] = value; }
    inline void set_texture(const std::string& mat_name, const texture& value) { m_textures[mat_name] = value; }


    int get_int(const std::string& mat_name) const {
        auto it = m_ints.find(mat_name);
        if (it != m_ints.end()) return it->second;
        return 0;
    }

    float get_float(const std::string& mat_name) const {
        auto it = m_floats.find(mat_name);
        if (it != m_floats.end()) return it->second;
        return 0.0;
    }

    vec2f get_vec2f(const std::string& mat_name) const {
        auto it = m_vec2f.find(mat_name);
        if (it != m_vec2f.end()) return it->second;
        return vec2f();
    }

    vec3f get_vec3f(const std::string& mat_name) const {
        auto it = m_vec3f.find(mat_name);
        if (it != m_vec3f.end()) return it->second;
        return vec3f();
    }

    texture get_texture(const std::string& mat_name) const {
        auto it = m_textures.find(mat_name);
        if (it != m_textures.end()) return it->second;
        return texture();
    }


    inline bool has_diffuse_map() const { return m_diffuse_map != nullptr; }
    inline bool has_normal_map() const { return m_normal_map != nullptr; }
    inline bool has_disp_map() const { return m_disp_map != nullptr; }
    inline bool has_info_map() const { return m_info_map != nullptr; }


    void set_all_uniforms(shader* shader_p);
private:
    const std::string m_DIFFUSE_NAME = "diffuse_map";
    const std::string m_NORMAL_NAME = "normal_map";
    const std::string m_DISP_NAME = "disp_map";
    const std::string m_INFO_NAME = "info_map";

    std::map<std::string, int> m_ints;
    std::map<std::string, float> m_floats;
    std::map<std::string, vec2f> m_vec2f;
    std::map<std::string, vec3f> m_vec3f;
    std::map<std::string, texture> m_textures;

    // info_map: r=transparency, g=specular, b=...
    texture *m_diffuse_map, *m_normal_map, *m_disp_map, *m_info_map;
};

#endif
