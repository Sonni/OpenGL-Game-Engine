#ifndef GAME_ENGINE_RENDERINGENGINE_H
#define GAME_ENGINE_RENDERINGENGINE_H

#include "../core/entity.h"


class rendering_engine
{
public:
    rendering_engine();
    void render(camera* cam, std::vector<entity*> e);
private:
};


#endif //GAME_ENGINE_RENDERINGENGINE_H
