#ifndef GAME_ENGINE_RENDERINGENGINE_H
#define GAME_ENGINE_RENDERINGENGINE_H

#include "../core_engine/entity.h"
#include "model/mesh.h"


class rendering_engine
{
public:
    rendering_engine();
    void render(camera* cam, std::vector<entity*> e) const;
    inline mesh* get_hud_mesh() const { return m_hud_mesh; }
private:
    mesh* m_hud_mesh;
};


#endif //GAME_ENGINE_RENDERINGENGINE_H
