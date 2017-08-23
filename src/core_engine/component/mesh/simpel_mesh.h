#ifndef INC_3D_ENGINE_SIMPEL_MESH_H
#define INC_3D_ENGINE_SIMPEL_MESH_H


#include "../../entity_component.h"
#include "../../../rendering_engine/model/mesh.h"
#include "../../../rendering_engine/texture.h"

class mesh_component : public entity_component
{
public:
    mesh_component(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const std::string& tex_file_name = "default.png", const std::string& normal_map = "");
    ~mesh_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void render() const;

private:
    mesh* _mesh;
    texture* tex;
    texture* depth_map;
    texture* normal_map;
    texture* disp_map;
    bool has_normal_map;

    mat4f* shadow_mvp;
};


#endif //INC_3D_ENGINE_SIMPEL_MESH_H
