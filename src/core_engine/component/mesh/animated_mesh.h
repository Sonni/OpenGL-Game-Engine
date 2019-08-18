#ifndef INC_3D_ENGINE_ANIMATED_MESH_H
#define INC_3D_ENGINE_ANIMATED_MESH_H

#include "../../../shader/shader.h"
#include "../../../rendering_engine/model/tna_loader.h"
#include "../../../rendering_engine/texture.h"
#include "../../../rendering_engine/model/mesh.h"


class animation_component : public entity_component
{
public:
    animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj = NULL, const std::string& tex_file_name = "default.png", const std::string& normal_map = "default_normal.png");
    ~animation_component();

    virtual void init();
    virtual void set_all_uni(camera& cam);
    virtual void process_input(const input& input, float delta);

    virtual void update(float delta, const camera &cam);
    virtual void render() const;

    inline physics_obj* get_physics_obj() const { return m_phy_obj; }

private:
    mesh* m_mesh;
    tna_model m_tna_model;
    texture* m_tex;
    physics_obj* m_phy_obj;
    bool m_draw = true;
    bool m_is_moving = false;

    texture* m_depth_map;
    mat4f* m_shadow_mvp;

    void set_joint_uniforms(joint* r) const;
};


#endif //INC_3D_ENGINE_ANIMATED_MESH_H
