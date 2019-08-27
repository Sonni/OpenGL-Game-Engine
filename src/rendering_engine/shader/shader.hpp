#ifndef GAME_ENGINE_SHADER_H
#define GAME_ENGINE_SHADER_H

#include <GL/glew.h>
#include <map>
#include <sstream>
#include "../../core_engine/camera.hpp"
#include "../light/phong_light.hpp"
#include "../texture.hpp"
#include "material.hpp"

class shader
{
public:
    shader(const std::string& vertex_file_name, const std::string& fragment_file_name, material* mat = new material());
    shader(const std::string& glsl_file_name, material* mat = new material());

    inline void use_shader() { glUseProgram(m_program_id); }

    inline void set_all_uni()
    {
        m_material->set_all_uniforms(this);
    }

    inline GLuint get_program_id() const { return m_program_id; }

    GLint get_uni_location(const std::string& name) const
    {
        return glGetUniformLocation(m_program_id, name.c_str());
    }

    inline void set_uniform_mat4f(const std::string &uniform_name, const mat4f &value)
    {
        glUniformMatrix4fv(get_uni_location(uniform_name), 1, GL_FALSE, &(value[0][0]));
    }

    inline void set_uniform_3f(const std::string &uniform_name, const vec3f &v)
    {
        glUniform3f(get_uniform(uniform_name), v.get_x(), v.get_y(), v.get_z());
    }

    inline void set_uniform_1i(const std::string& uniform_name, int i)
    {
        glUniform1i(get_uniform(uniform_name), i);
    }

    inline void set_uniform_1f(const std::string& uniform_name, float f)
    {
        glUniform1f(get_uniform(uniform_name), f);
    }

    inline void set_uniform_2f(const std::string& uniform_name, const vec2f& v)
    {
        glUniform2f(get_uniform(uniform_name), v.get_x(), v.get_y());
    }

    inline void set_uniform_4f(const std::string& uniform_name, const vec4f& v)
    {
        glUniform4f(get_uniform(uniform_name), v.get_x(), v.get_y(), v.get_z(), v.get_w());
    }

    inline void set_sampler_2d(const std::string& uniform_name, texture* tex, int i)
    {
        tex->bind(i);
        set_uniform_1i(uniform_name, i);
    }
    
    void add_light(phong_light *l) { m_lc = l; }

    void set_light_loc();
    
    void set_light();

    inline void add_uniform(const std::string& name) { m_uniforms[name] = get_uni_location(name); }
    inline GLint get_uniform(const std::string& name) { return m_uniforms[name]; }

    inline material* get_material() const 			  { return m_material; }



private:
    std::string m_path = "../res/glsl/";

    GLuint m_program_id;
    std::map<std::string, GLint> m_uniforms;

    phong_light* m_lc;

    material* m_material;


    std::string get_code(const std::string& file_name);
    GLuint load(const std::string& vertex_file_path, const std::string& fragment_file_path);
};


#endif //GAME_ENGINE_SHADER_H
