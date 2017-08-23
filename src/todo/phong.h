#ifndef INC_3D_ENGINE_PHONG_H
#define INC_3D_ENGINE_PHONG_H


#include <sstream>
#include <vector>
#include "../core_engine/math/vector.h"
#include "lighting.h"
#include "../core_engine/math/matrix.h"
#include "../shader/shader.h"
#include "../rendering_engine/model/mesh.h"
#include "../rendering_engine/texture.h"

class phong : public entity_component
{
public:
    phong(mesh* _mesh, mat4f* shadowMap, texture* depth_map) :
            _mesh(_mesh), s_directionalLight(directional_light(base_light(vec3f(1,1,1),0.6f), vec3f(1,1,1)))
            , s_ambientLight(vec3f(0.3, 0.3, 0.3))
    {


        s_pointLights.push_back(point_light(base_light(vec3f(0, 1, 0), 20.9f), attenuation(0, 0, 1), vec3f(30, 12, 30), 100));
        s_spotLights.push_back(spot_light(point_light(base_light(vec3f(0,1,1), 0.8f), attenuation(0,0,0.1f), vec3f(30, 10, 30), 30),vec3f(1,1,1).normalized(),0.8f));
        s_numPointLights = 1;
        s_numSpotLights = 1;

        tex = new texture("bricks.jpg");
        normal = new texture("normal/bricks_normal.jpg");
        disp_map = new texture("disp/bricks_disp.jpg");

        this->shadow_mvp = shadowMap;
        this->depth_map = depth_map;



    }
    virtual void init()
    {
        get_shader()->add_uniform("transform");
        get_shader()->add_uniform("transformProjected");
        get_shader()->add_uniform("baseColor");
        get_shader()->add_uniform("ambientLight");

        get_shader()->add_uniform("specularIntensity");
        get_shader()->add_uniform("specularPower");
        get_shader()->add_uniform("eyePos");

        get_shader()->add_uniform("directionalLight.base.color");
        get_shader()->add_uniform("directionalLight.base.intensity");
        get_shader()->add_uniform("directionalLight.direction");

        for(int i = 0; i < MAX_POINT_LIGHTS; i++)
        {
            std::ostringstream pointLightNameBuilder;
            pointLightNameBuilder << "pointLights[" << i << "]";
            std::string pointLightName = pointLightNameBuilder.str();
            //std::string pointLightName = "pointLights[" + std::to_string(i) + "]";

            get_shader()->add_uniform(pointLightName + ".base.color");
            get_shader()->add_uniform(pointLightName + ".base.intensity");
            get_shader()->add_uniform(pointLightName + ".atten.constant");
            get_shader()->add_uniform(pointLightName + ".atten.linear");
            get_shader()->add_uniform(pointLightName + ".atten.exponent");
            get_shader()->add_uniform(pointLightName + ".position");
            get_shader()->add_uniform(pointLightName + ".range");
        }

        for(int i = 0; i < MAX_SPOT_LIGHTS; i++)
        {
            std::ostringstream spotLightNameBuilder;
            spotLightNameBuilder << "spotLights[" << i << "]";
            std::string spotLightName = spotLightNameBuilder.str();
            //std::string spotLightName = "spotLights[" + std::to_string(i) + "]";

            get_shader()->add_uniform(spotLightName + ".pointLight.base.color");
            get_shader()->add_uniform(spotLightName + ".pointLight.base.intensity");
            get_shader()->add_uniform(spotLightName + ".pointLight.atten.constant");
            get_shader()->add_uniform(spotLightName + ".pointLight.atten.linear");
            get_shader()->add_uniform(spotLightName + ".pointLight.atten.exponent");
            get_shader()->add_uniform(spotLightName + ".pointLight.position");
            get_shader()->add_uniform(spotLightName + ".pointLight.range");
            get_shader()->add_uniform(spotLightName + ".direction");
            get_shader()->add_uniform(spotLightName + ".cutoff");
        }

        get_shader()->add_uniform("sampler");
        get_shader()->add_uniform("normal_map");
        get_shader()->add_uniform("disp_tex");

        get_shader()->add_uniform("shadow_mvp");
        get_shader()->add_uniform("shadow_tex");
    }

    virtual void set_all_uni(camera& cam)
    {

        mat4f mvp = cam.get_view_projection() * get_transform()->get_transformation();

        get_shader()->set_uniform_mat4f("transformProjected", mvp);
        get_shader()->set_uniform_mat4f("transform", get_transform()->get_transformation());
        get_shader()->set_uniform_3f("baseColor", vec3f(1, 1, 1));

        get_shader()->set_uniform_3f("ambientLight", s_ambientLight);
        get_shader()->set_uniform_3f("directionalLight.direction", s_directionalLight.direction);
        get_shader()->set_uniform_3f("directionalLight.base.color", s_directionalLight.base.color);
        get_shader()->set_uniform_1f("directionalLight.base.intensity", s_directionalLight.base.intensity);

        for(int i = 0; i < s_numPointLights; i++)
        {
            std::ostringstream pointLightNameBuilder;
            pointLightNameBuilder << "pointLights[" << i << "]";
            std::string pointLightName = pointLightNameBuilder.str();

            get_shader()->set_uniform_3f(pointLightName + ".base.color", s_pointLights[i].base.color);
            get_shader()->set_uniform_1f(pointLightName + ".base.intensity", s_pointLights[i].base.intensity);
            get_shader()->set_uniform_1f(pointLightName + ".atten.constant", s_pointLights[i].atten.constant);
            get_shader()->set_uniform_1f(pointLightName + ".atten.linear", s_pointLights[i].atten.linear);
            get_shader()->set_uniform_1f(pointLightName + ".atten.exponent", s_pointLights[i].atten.exponent);
            get_shader()->set_uniform_3f(pointLightName + ".position", s_pointLights[i].position);
            get_shader()->set_uniform_1f(pointLightName + ".range", s_pointLights[i].range);

        }

        for(int i = 0; i < s_numSpotLights; i++)
        {
            std::ostringstream spotLightNameBuilder;
            spotLightNameBuilder << "spotLights[" << i << "]";
            std::string spotLightName = spotLightNameBuilder.str();

            get_shader()->set_uniform_3f(spotLightName + ".pointLight.base.color", s_spotLights[i]._point_light.base.color);
            get_shader()->set_uniform_1f(spotLightName + ".pointLight.base.intensity", s_spotLights[i]._point_light.base.intensity);
            get_shader()->set_uniform_1f(spotLightName + ".pointLight.atten.constant", s_spotLights[i]._point_light.atten.constant);
            get_shader()->set_uniform_1f(spotLightName + ".pointLight.atten.linear", s_spotLights[i]._point_light.atten.linear);
            get_shader()->set_uniform_1f(spotLightName + ".pointLight.atten.exponent", s_spotLights[i]._point_light.atten.exponent);
            get_shader()->set_uniform_3f(spotLightName + ".pointLight.position", s_spotLights[i]._point_light.position);
            get_shader()->set_uniform_3f(spotLightName + ".pointLight.position", s_spotLights[i]._point_light.range);
            get_shader()->set_uniform_3f(spotLightName + ".direction", s_spotLights[i].direction);
            get_shader()->set_uniform_1f(spotLightName + ".cutoff", s_spotLights[i].cutoff);
        }

        get_shader()->set_uniform_1f("specularIntensity", 1.0f);
        get_shader()->set_uniform_1f("specularPower", 8.0f);

        get_shader()->set_uniform_3f("eyePos", *cam.get_transform()->get_pos());

        tex->bind(0);
        get_shader()->set_uniform_1i("sampler", 0);

        normal->bind(1);
        get_shader()->set_uniform_1i("normal_map", 1);

        disp_map->bind(2);
        get_shader()->set_uniform_1i("disp_tex", 2);

        get_shader()->set_uniform_mat4f("shadow_mvp", *shadow_mvp);

        depth_map->bind(3, true);
        get_shader()->set_uniform_1i("shadow_tex", 3);
    }

    virtual void update(float delta)
    {

    }

    virtual void render() const
    {
        _mesh->draw();
    }
    
private:
    int MAX_POINT_LIGHTS = 1;
    int MAX_SPOT_LIGHTS = 1;
    vec3f s_ambientLight;
    directional_light s_directionalLight;
    std::vector<point_light> s_pointLights;
    std::vector<spot_light> s_spotLights;
    int s_numPointLights;
    int s_numSpotLights;

    texture* tex;
    texture* normal;
    texture* disp_map;

    mesh* _mesh;

    texture* depth_map;

    mat4f* shadow_mvp;

};


#endif //INC_3D_ENGINE_PHONG_H
