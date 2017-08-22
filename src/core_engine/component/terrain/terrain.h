#ifndef ___D_rendering_engine__terrain__
#define ___D_rendering_engine__terrain__


#include "../../entity_component.h"
#include "../../../rendering_engine/texture.h"
#include "../../../rendering_engine/model/mesh.h"
#include "perlin_noise.h"

class terrain_component : public entity_component
{
public:
    
    terrain_component(mat4f* shadowMap, texture* depth_map, int _grid_x, int _grid_z, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap);
    void operator=(const terrain_component& terrain) {}
    
    ~terrain_component()
    {
        delete _mesh;
    }

    virtual void init();
    virtual void update(float delta);
    virtual void render() const;
    virtual void set_all_uni(camera& cam);

    float barry_centric(const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec2f& pos) const;
    float get_terrain_y(float world_x, float world_z);
private:

    const int SIZE = 200;

    int grid_x, grid_z;
    std::vector<std::vector<float>> heights;

    std::vector<texture*> shadows;
    texture* cur;

    mesh* _mesh;

    texture* background;
    texture* r_tex;
    texture* g_tex;
    texture* b_tex;
    texture* blend_map;

    texture* depth_map;
    mat4f* shadow_mvp;

    float get_height(int x, int z);

    void load_mesh();
    void load_raw_heights();
    void load_noise_heights();
    void load_height_map();
};

#endif