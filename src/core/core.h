#ifndef GAME_ENGINE_CORE_H
#define GAME_ENGINE_CORE_H

#include "io/window.h"
#include "../rendering_engine/rendering_engine.h"

class core
{
public:
    core(window* Window, rendering_engine* renderingEngine);
    bool run();
private:
    window* Window;
    rendering_engine* renderingEngine;





};


#endif //GAME_ENGINE_CORE_H
