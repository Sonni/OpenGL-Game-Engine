#include "entity_component.hpp"
#include "../rendering_engine/shader/shader.hpp"

material* entity_component::get_material() const { return m_parent->get_shader()->get_material(); }
