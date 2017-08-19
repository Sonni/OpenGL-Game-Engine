#ifndef GAME_ENGINE_SHADER_H
#define GAME_ENGINE_SHADER_H

#include <GL/glew.h>
#include <fstream>
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

    GLint get_uni_location(const GLchar* name) const
    {
        return glGetUniformLocation(program_id, name);
    }

    inline void SetUniformMat4(const std::string& uniformName, const mat4f& value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(program_id, uniformName.c_str()), 1, GL_FALSE, &(value[0][0]));
    }

    void get_light_loc();
    void set_light() const;

    inline void add_lights(std::vector<light>* light) { lights = light;}



private:
    GLuint load(std::string vertex_file_path, std::string fragment_file_path);
    std::vector<GLint> light_pos, light_color, attenuation;
    std::vector<light>* lights = nullptr;

    GLuint program_id;
};


#endif //GAME_ENGINE_SHADER_H
