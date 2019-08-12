#ifndef ___D_rendering_engine__terrain__
#define ___D_rendering_engine__terrain__


#include "../../entity_component.h"
#include "../../../rendering_engine/texture.h"
#include "../../../rendering_engine/model/mesh.h"
#include "terrain_block.h"
#include "../../../physics_engine/collider/sphere.h"


class terrain_component : public entity_component
{
public:

    terrain_component(mat4f* shadowMap, texture* depth_map, int _grid_x, int _grid_z, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap, const int lod);
    void operator=(const terrain_component& terrain) {}

    ~terrain_component()
    {
        delete _sphere;
    }

    virtual void init();
    virtual void update(float delta, const camera &cam);
    virtual void render() const;
    virtual void process_input(const input& input, float delta);

    virtual void set_all_uni(camera& cam);

    sphere* get_sphere() const { return _sphere; }
    const sphere& get_sphere() { return *_sphere; }

    float barry_centric(const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec2f& pos) const;
    float get_terrain_y(float world_x, float world_z);
private:

    const int SIZE = 100;
    int num_vertices = 4; //256
    int lod = 1;
    float max_y, min_y;
    sphere* _sphere;
    bool draw = true;
    bool draw_line_mode = true;

    int grid_x, grid_z;
    std::vector<std::vector<float>> heights;

    std::vector<texture*> shadows;
    texture* cur;

    std::vector<std::vector<terrain_block*>> blocks;

    texture* background;
    texture* r_tex;
    texture* g_tex;
    texture* b_tex;
    texture* blend_map;

    texture* depth_map;
    mat4f* shadow_mvp;


    void load_raw_heights(const std::string &path);
    void load_noise_heights(const int vertices);
    void load_height_map();
    void setup_viewbox();
    void setup_caustic();
    void update_caustic();
    void connect_seams(terrain_block* block);

};

#endif