#ifndef GAME_ENGINE_SHADER_H
#define GAME_ENGINE_SHADER_H

#include <GL/glew.h>
#include <fstream>
#include <map>
#include <sstream>
#include "../core_engine/transform.h"
#include "../core_engine/camera.h"
#include "../rendering_engine/light/phong_light.h"


class shader
{
public:
    shader(std::string vertex_file_name, std::string fragment_file_name);
    shader(std::string glsl_file_name);

    inline void use_shader() { glUseProgram(program_id); }

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
    
    void add_light(phong_light *l) { this->lc = l; }

    void set_light_loc()
    {
        add_uniform("directionalLight.base.color");
        add_uniform("directionalLight.base.intensity");
        add_uniform("directionalLight.direction");

        for(int i = 0; i < lc->MAX_POINT_LIGHTS; i++)
        {
            std::ostringstream pointLightNameBuilder;
            pointLightNameBuilder << "pointLights[" << i << "]";
            std::string pointLightName = pointLightNameBuilder.str();

            add_uniform(pointLightName + ".base.color");
            add_uniform(pointLightName + ".base.intensity");
            add_uniform(pointLightName + ".atten.constant");
            add_uniform(pointLightName + ".atten.linear");
            add_uniform(pointLightName + ".atten.exponent");
            add_uniform(pointLightName + ".position");
            add_uniform(pointLightName + ".range");
        }

        for(int i = 0; i < lc->MAX_SPOT_LIGHTS; i++)
        {
            std::ostringstream spotLightNameBuilder;
            spotLightNameBuilder << "spotLights[" << i << "]";
            std::string spotLightName = spotLightNameBuilder.str();

            add_uniform(spotLightName + ".pointLight.base.color");
            add_uniform(spotLightName + ".pointLight.base.intensity");
            add_uniform(spotLightName + ".pointLight.atten.constant");
            add_uniform(spotLightName + ".pointLight.atten.linear");
            add_uniform(spotLightName + ".pointLight.atten.exponent");
            add_uniform(spotLightName + ".pointLight.position");
            add_uniform(spotLightName + ".pointLight.range");
            add_uniform(spotLightName + ".direction");
            add_uniform(spotLightName + ".cutoff");
        }

    }
    
    void set_light()
    {
        set_uniform_3f("directionalLight.direction", lc->s_directionalLight.direction);
        set_uniform_3f("directionalLight.base.color", lc->s_directionalLight.base.color);
        set_uniform_1f("directionalLight.base.intensity", lc->s_directionalLight.base.intensity);

        for(int i = 0; i < lc->s_numPointLights; i++)
        {
            std::ostringstream pointLightNameBuilder;
            pointLightNameBuilder << "pointLights[" << i << "]";
            std::string pointLightName = pointLightNameBuilder.str();

            set_uniform_3f(pointLightName + ".base.color", lc->s_pointLights[i].base.color);
            set_uniform_1f(pointLightName + ".base.intensity", lc->s_pointLights[i].base.intensity);
            set_uniform_1f(pointLightName + ".atten.constant", lc->s_pointLights[i].atten.constant);
            set_uniform_1f(pointLightName + ".atten.linear", lc->s_pointLights[i].atten.linear);
            set_uniform_1f(pointLightName + ".atten.exponent", lc->s_pointLights[i].atten.exponent);
            set_uniform_3f(pointLightName + ".position", lc->s_pointLights[i].position);
            set_uniform_1f(pointLightName + ".range", lc->s_pointLights[i].range);

        }

        for(int i = 0; i < lc->s_numSpotLights; i++)
        {
            std::ostringstream spotLightNameBuilder;
            spotLightNameBuilder << "spotLights[" << i << "]";
            std::string spotLightName = spotLightNameBuilder.str();

            set_uniform_3f(spotLightName + ".pointLight.base.color", lc->s_spotLights[i]._point_light.base.color);
            set_uniform_1f(spotLightName + ".pointLight.base.intensity", lc->s_spotLights[i]._point_light.base.intensity);
            set_uniform_1f(spotLightName + ".pointLight.atten.constant", lc->s_spotLights[i]._point_light.atten.constant);
            set_uniform_1f(spotLightName + ".pointLight.atten.linear", lc->s_spotLights[i]._point_light.atten.linear);
            set_uniform_1f(spotLightName + ".pointLight.atten.exponent", lc->s_spotLights[i]._point_light.atten.exponent);
            set_uniform_3f(spotLightName + ".pointLight.position", lc->s_spotLights[i]._point_light.position);
            set_uniform_1f(spotLightName + ".pointLight.range", lc->s_spotLights[i]._point_light.range);
            set_uniform_3f(spotLightName + ".direction", lc->s_spotLights[i].q.get_forward());
            set_uniform_1f(spotLightName + ".cutoff", lc->s_spotLights[i].cutoff);

        }
    }


    inline void add_uniform(const std::string& name) { uniforms[name] = get_uni_location(name); }
    inline GLint get_uniform(const std::string& name) { return uniforms[name]; }



private:
    GLuint load(std::string vertex_file_path, std::string fragment_file_path);
    phong_light* lc;

    std::map<std::string, GLint> uniforms;


    GLuint program_id;
};


#endif //GAME_ENGINE_SHADER_H
