#ifndef INC_3D_ENGINE_PHONG_H
#define INC_3D_ENGINE_PHONG_H

#include "../../math/vector.h"
#include "../../../shader/shader.h"
#include "../../../rendering_engine/model/mesh.h"
#include "../../../rendering_engine/texture.h"
#include "../../../physics_engine/physics_obj.h"

class advanced_mesh : public entity_component
{
public:
    advanced_mesh(mesh* _mesh, mat4f* shadowMap, texture* depth_map, const vec3f& ambient, physics_obj* phy_obj = NULL, const std::string& tex_name = "default.png", const std::string& normal_name = "default_normal.jpg", const std::string& disp_name = "default_disp.png", const std::string& specular_name = "default.png");

    virtual void init();
    virtual void set_all_uni(camera& cam);

    virtual void update(float delta, const camera &cam);
    virtual void render() const;

    inline physics_obj* get_physics_obj() const { return m_phy_obj; }


private:
    vec3f m_s_ambientLight;

    texture* m_tex;
    texture* m_normal;
    texture* m_disp_map;
    texture* m_specular_map;

    mesh* m_mesh;
    physics_obj* m_phy_obj;
    bool m_draw = true;

    texture* m_depth_map;
    mat4f* m_shadow_mvp;
};


#endif //INC_3D_ENGINE_PHONG_H
