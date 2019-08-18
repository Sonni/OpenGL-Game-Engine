#ifndef INC_3D_ENGINE_SIMPEL_MESH_H
#define INC_3D_ENGINE_SIMPEL_MESH_H


#include "../../entity_component.h"
#include "../../../rendering_engine/model/mesh.h"
#include "../../../rendering_engine/texture.h"

class mesh_component : public entity_component
{
public:
    mesh_component(mesh* _mesh, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj, const vec3f& ambient, const std::string& tex_file_name = "default.png");
    ~mesh_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void update(float delta, const camera &cam);
    virtual void render() const;

private:
    mesh* m_mesh;
    texture* m_tex;
    texture* m_depth_map;
    vec3f m_ambient;

    physics_obj* m_phy_obj;
    bool m_draw = true;

    mat4f* m_shadow_mvp;
};


#endif //INC_3D_ENGINE_SIMPEL_MESH_H
