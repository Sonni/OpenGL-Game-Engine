#ifndef GAME_ENGINE_SHADER_H
#define GAME_ENGINE_SHADER_H

#include <GL/glew.h>
#include <fstream>
#include <map>
#include "../core_engine/transform.h"
#include "../core_engine/camera.h"
#include "../rendering_engine/light.h"


class shader
{
public:
    shader(std::string vertex_file_name, std::string fragment_file_name);
    shader(std::string glsl_file_name);
    void use_shader()
    {
        glUseProgram(program_id);
    }

    virtual void get_all_uni_loc() {}
    virtual void set_all_uni(transform& Transform, camera& cam) {}

    inline GLuint get_program_id() const {return program_id;}

    GLint get_uni_location(const std::string& name) const
    {
        return glGetUniformLocation(program_id, name.c_str());
    }

    inline void set_uniform_mat4f(const std::string &uniform_name, const mat4f &value)
    {
        glUniformMatrix4fv(get_uni_location(uniform_name), 1, GL_FALSE, &(value[0][0]));
    }

    inline void set_uniform_3f(const std::string &uniform_name, const vec3f &v)
    {
        glUniform3f(get_uniform(uniform_name), v.get_x(), v.get_y(), v.get_z());
    }

    inline void set_uniform_1i(const std::string& uniform_name, int f)
    {
        glUniform1i(get_uniform(uniform_name), f);
    }

    inline void set_uniform_2f(const std::string& uniform_name, const vec2f& v)
    {
        glUniform2f(get_uniform(uniform_name), v.get_x(), v.get_y());
    }

    inline void set_uniform_4f(const std::string& uniform_name, const vec4f& v)
    {
        glUniform4f(get_uniform(uniform_name), v.get_x(), v.get_y(), v.get_z(), v.get_w());
    }

    void get_light_loc();
    void set_light() const;

    inline void add_lights(std::vector<light>* light) { lights = light;}

    inline void add_uniform(const std::string& name) { uniforms[name] = get_uni_location(name); }
    inline GLint get_uniform(const std::string& name) { return uniforms[name]; }



private:
    GLuint load(std::string vertex_file_path, std::string fragment_file_path);
    std::vector<GLint> light_pos, light_color, attenuation;
    std::vector<light>* lights = nullptr;

    std::map<std::string, GLint> uniforms;


    GLuint program_id;
};


#endif //GAME_ENGINE_SHADER_H
