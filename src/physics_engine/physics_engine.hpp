#ifndef GAME_ENGINE_PHYSICS_ENGINE_H
#define GAME_ENGINE_PHYSICS_ENGINE_H


#include "../core_engine/component/terrain/terrain.hpp"
#include "physics_obj.hpp"
#include "collider/aabb.hpp"

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

    void update_objs(entity* e, physics_obj* obj, std::vector<physics_obj*> objs)
    {
        obj->get_collider()->set_pos(*e->get_transform()->get_pos()); //For test purpose

        for (physics_obj* o : objs)
        {
            collider* c1 = obj->get_collider();
            collider* c2 = o->get_collider();

            intersect_data data = c1->intersect(*c2);

            if (data.get_does_intersect())
            {
                //std::cout << data.get_direction() << std::endl;
                vec3f new_pos = *e->get_transform()->get_pos() + data.get_direction();
                e->get_transform()->set_pos(new_pos);
            }
        }
    }

    int offset = 0;
    void update_terrain(std::vector<terrain_component*> terrain, std::vector<entity*> objs)
    {
        for (terrain_component* t : terrain)
        {
            for (entity *e : objs)
            {
                vec3f pos = *e->get_transform()->get_pos();
                float y = t->get_terrain_y(e->get_transform()->get_pos()->get_x(),
                                           e->get_transform()->get_pos()->get_z());

                if (y > -1000 && y + offset > pos.get_y()) {
                    e->get_transform()->get_pos()->set(pos.get_x(), y + offset, pos.get_z());
                }
            }
        }
    }

};


#endif //GAME_ENGINE_PHYSICS_ENGINE_H
