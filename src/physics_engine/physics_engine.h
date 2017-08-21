#ifndef GAME_ENGINE_PHYSICS_ENGINE_H
#define GAME_ENGINE_PHYSICS_ENGINE_H


#include "../core_engine/component/terrain/terrain.h"
#include "physics_obj.h"
#include "collider/aabb.h"

class physics_engine
{
public:
    physics_engine()
    {

    }

    void update_gravity(std::vector<entity*> objs, float delta)
    {
        for (entity* o : objs)
        {
            o->get_transform()->get_pos()->set_y(o->get_transform()->get_pos()->get_y() - (10.0f * delta));
        }
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

    void update_terrain(std::vector<terrain_component*> terrain, std::vector<entity*> objs)
    {
        for (terrain_component* t : terrain)
        {
            for (entity *e : objs)
            {
                vec3f pos = *e->get_transform()->get_pos();
                float y = t->get_terrain_y(e->get_transform()->get_pos()->get_x(),
                                           e->get_transform()->get_pos()->get_z());

                if (y > -1000 && y + 5 > pos.get_y()) {
                    e->get_transform()->get_pos()->set(pos.get_x(), y + 5, pos.get_z());
                }
            }
        }
    }

};


#endif //GAME_ENGINE_PHYSICS_ENGINE_H
