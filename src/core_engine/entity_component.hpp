#ifndef GAME_ENGINE_ENTITY_COMPONENT_H
#define GAME_ENGINE_ENTITY_COMPONENT_H

#include "entity.hpp"
#include "io/input.hpp"
#include "../physics_engine/physics_obj.hpp"
#include "../rendering_engine/shader/material.hpp"

class camera;
class entity_component
{
public:
    entity_component() :
			m_parent(0)
	{}
	virtual ~entity_component() {}

    virtual void init() {}
	virtual void process_input(const input &_input, float delta) {}
	virtual void update(float delta, const camera &cam) {}
	virtual void render() const {}

	virtual void set_all_uni(camera& cam) {}




	void delete_component(entity_component *component) { m_parent->delete_component(component); }

	inline transform* get_transform()             { return m_parent->get_transform(); }
	inline const transform& get_transform() const { return *m_parent->get_transform(); }
    inline shader* get_shader() const 			  { return m_parent->get_shader(); }
    material* get_material() const;

	virtual void set_parent(entity* parent) { m_parent = parent; }
private:
	entity* m_parent;

    entity_component(const entity_component& other) {}
	void operator=(const entity_component& other) {}
};

#endif