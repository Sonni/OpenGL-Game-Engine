#include "entity.h"
#include "entity_component.h"

entity::~entity()
{
    for(unsigned int i = 0; i < m_components.size(); i++)
    {
        if(m_components[i])
        {
            delete m_components[i];
        }
    }
}

entity* entity::add_component(entity_component *component)
{
    m_components.push_back(component);
    component->set_parent(this);
    component->init();
    return this;
}

void entity::delete_component(entity_component *component)
{
    for (unsigned int i = 0; i < m_components.size(); i++)
        if (component == m_components[i])
        {
            m_components.erase(m_components.begin()+i);
            break;
        }
}

void entity::set_all_uni(camera& cam)
{
    for(unsigned int i = 0; i < m_components.size(); i++)
    {
        m_components[i]->set_all_uni(cam);
    }
}

void entity::process_input(const input &input, float delta)
{
    for(unsigned int i = 0; i < m_components.size(); i++)
    {
        m_components[i]->process_input(input, delta);
    }
}

void entity::update(float delta, const camera &cam)
{
    for(unsigned int i = 0; i < m_components.size(); i++)
    {
        m_components[i]->update(delta, cam);
    }
}

void entity::render() const
{
    for(unsigned int i = 0; i < m_components.size(); i++)
    {
        m_components[i]->render();
    }
}

