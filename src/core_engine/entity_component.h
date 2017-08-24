#ifndef GAME_ENGINE_ENTITY_COMPONENT_H
#define GAME_ENGINE_ENTITY_COMPONENT_H

#include "entity.h"
#include "io/input.h"
#include "../physics_engine/physics_obj.h"

class camera;
class entity_component
{
public:
    entity_component() :
			parent(0)
	{}
	virtual ~entity_component() {}

    virtual void init() {}
	virtual void process_input(const input &_input, float delta) {}
	virtual void update(float delta, const camera &cam) {}
	virtual void render() const {}

	virtual void set_all_uni(camera& cam) {}




	void delete_component(entity_component *component)  {parent->delete_component(component);}

	inline transform* get_transform()             { return parent->get_transform(); }
	inline const transform& get_transform() const { return *parent->get_transform(); }
	inline shader* get_shader() const {return parent->get_shader();}

	virtual void set_parent(entity* parent) { this->parent = parent; }
private:
	entity* parent;

    entity_component(const entity_component& other) {}
	void operator=(const entity_component& other) {}
};

#endif