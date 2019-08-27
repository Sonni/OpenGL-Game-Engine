#ifndef INC_3D_ENGINE_LIGHT_COMPO_H
#define INC_3D_ENGINE_LIGHT_COMPO_H

#include "lighting.hpp"
#include <vector>

class phong_light
{
public:
    phong_light() : m_directionalLight(directional_light(base_light(vec3f(1,1,1),0.6f), vec3f(1,1,1)))
    {
        m_numPointLights = 1;
        m_numSpotLights = 1;
    }

    int m_MAX_POINT_LIGHTS = 1;
    int m_MAX_SPOT_LIGHTS = 1;
    directional_light m_directionalLight;
    std::vector<point_light> m_pointLights;
    std::vector<spot_light> m_spotLights;
    int m_numPointLights;
    int m_numSpotLights;
};


#endif //INC_3D_ENGINE_LIGHT_COMPO_H
