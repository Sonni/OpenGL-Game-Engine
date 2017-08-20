#include <iostream>
#include "core_engine/core_engine.h"
#include "core_engine/io/sound.h"

int main(int argc, char *argv[])
{
    window* main_window = new window(1440, 900, "Sonni's 3D Game Engine");
    rendering_engine* graphics_engine = new rendering_engine();
    physics_engine* physicsEngine = new physics_engine();

    core_engine* _core_engine = new core_engine(main_window, graphics_engine, physicsEngine);
    _core_engine->run();

    delete main_window;
    delete graphics_engine;
    delete _core_engine;
    delete physicsEngine;

    return 0;
}