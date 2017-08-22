#include "entity.h"
#include "entity_component.h"

entity::~entity()
{
    for(unsigned int i = 0; i < components.size(); i++)
    {
        if(components[i])
        {
            delete components[i];
        }
    }
    delete shader_program;
}

entity* entity::add_component(entity_component *component)
{
    components.push_back(component);
    component->set_parent(this);
    component->init();
    return this;
}

void entity::delete_component(entity_component *component)
{
    for (unsigned int i = 0; i < components.size(); i++)
        if (component == components[i])
        {
            components.erase(components.begin()+i);
            break;
        }
}

void entity::set_all_uni(camera& cam)
{
    for(unsigned int i = 0; i < components.size(); i++)
    {
        components[i]->set_all_uni(cam);
    }
}

void entity::process_input(const input &input, float delta)
{
    for(unsigned int i = 0; i < components.size(); i++)
    {
        components[i]->process_input(input, delta);
    }
}

void entity::update(float delta)
{
    for(unsigned int i = 0; i < components.size(); i++)
    {
        components[i]->update(delta);
    }
}

void entity::render() const
{
    for(unsigned int i = 0; i < components.size(); i++)
    {
        components[i]->render();
    }
}

