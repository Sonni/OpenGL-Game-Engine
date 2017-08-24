#ifndef INC_3D_ENGINE_LIGHT_COMPO_H
#define INC_3D_ENGINE_LIGHT_COMPO_H


#include "lighting.h"
#include <vector>

class phong_light
{
public:
    phong_light() :
        s_directionalLight(directional_light(base_light(vec3f(1,1,1),0.6f), vec3f(1,1,1)))
    {
        s_numPointLights = 1;
        s_numSpotLights = 1;
    }

    int MAX_POINT_LIGHTS = 1;
    int MAX_SPOT_LIGHTS = 1;
    directional_light s_directionalLight;
    std::vector<point_light> s_pointLights;
    std::vector<spot_light> s_spotLights;
    int s_numPointLights;
    int s_numSpotLights;
};


#endif //INC_3D_ENGINE_LIGHT_COMPO_H
