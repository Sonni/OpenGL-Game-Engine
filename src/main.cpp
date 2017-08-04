#include <iostream>
#include "Core/core.h"

int main(int argc, char *argv[])
{
    window* main_window = new window(1440, 900, "Sonnis 3D Game Engine");
    rendering_engine* graphics_engine = new rendering_engine();

    core* core_engine = new core(main_window, graphics_engine);
    core_engine->run();

    delete main_window;
    delete graphics_engine;
    delete core_engine;

    return 0;
}