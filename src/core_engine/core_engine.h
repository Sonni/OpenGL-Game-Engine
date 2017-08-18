#ifndef GAME_ENGINE_CORE_H
#define GAME_ENGINE_CORE_H

#include "io/window.h"
#include "../rendering_engine/rendering_engine.h"
#include "component/water/water_fbo.h"

class core_engine
{
public:
    core_engine(window* Window, rendering_engine* renderingEngine);
    bool run();
private:
    window* Window;
    rendering_engine* renderingEngine;

    void setup_reflection(camera* cam, water_fbo* wfb, float height_of_water, float y_distance);
    void setup_refraction(camera* cam, water_fbo* wfb, float height_of_water, float y_distance, quaternion old_cam_rot);




};


#endif //GAME_ENGINE_CORE_H
