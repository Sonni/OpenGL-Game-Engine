#ifndef INC_3D_ENGINE_ANIMATED_MESH_H
#define INC_3D_ENGINE_ANIMATED_MESH_H

#include "../../entity_component.h"
#include "../../../rendering_engine/model/mesh.h"
#include "../../../rendering_engine/model/tnaLoader.h"
#include "../../../rendering_engine/texture.h"
#include "../../../shader/shader.h"
#include "../../../physics_engine/physics_obj.h"
#include <sstream>


class animation_component : public entity_component
{
public:
    animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj = NULL, const std::string& tex_file_name = "default.png", const std::string& normal_map = "default_normal.png");
    ~animation_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);

    virtual void update(float delta, const camera &cam);
    virtual void render() const;

    physics_obj* get_physics_obj() const { return phy_obj; }


protected:
private:
    mesh _mesh;
    TNAModel tna_model;
    texture* tex;
    physics_obj* phy_obj;
    bool draw = true;

    texture* depth_map;
    mat4f* shadow_mvp;
};


#endif //INC_3D_ENGINE_ANIMATED_MESH_H
