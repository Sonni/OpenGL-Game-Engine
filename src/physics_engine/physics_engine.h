#ifndef GAME_ENGINE_PHYSICS_ENGINE_H
#define GAME_ENGINE_PHYSICS_ENGINE_H


#include "../core_engine/component/terrain.h"
#include "physics_obj.h"
#include "collider/aabb.h"

class physics_engine
{
public:
    physics_engine()
    {

    }

    void update_objs(physics_obj* obj, std::vector<physics_obj*> objs)
    {
        for (physics_obj* o : objs)
        {
            collider* c1 = obj->get_collider();
            collider* c2 = o->get_collider();

            intersect_data data = c1->intersect(*c2);
            if (data.get_does_intersect())
            {
                vec3f new_pos = *obj->get_transform()->get_pos() + data.get_direction();
                obj->get_transform()->set_pos(new_pos);
            }
        }
    }

    void update_terrain(terrain_component* terrain, std::vector<entity*> objs)
    {
        for (entity* e : objs)
        {
            vec3f pos = *e->get_transform()->get_pos();
            float y = terrain->getHeightOfTerrain(e->get_transform()->get_pos()->get_x(), e->get_transform()->get_pos()->get_z());
            if (y > 0)
            {
                e->get_transform()->get_pos()->set(pos.get_x(), y+5, pos.get_z());
            }
        }

    }

};


#endif //GAME_ENGINE_PHYSICS_ENGINE_H
