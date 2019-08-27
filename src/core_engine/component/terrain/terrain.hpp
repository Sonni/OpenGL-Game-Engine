#ifndef ___D_rendering_engine__terrain__
#define ___D_rendering_engine__terrain__


#include "../../entity_component.hpp"
#include "../../../rendering_engine/texture.hpp"
#include "../../../rendering_engine/model/mesh.hpp"
#include "terrain_block.hpp"
#include "../../../physics_engine/collider/sphere.hpp"
#include "quad_tree.hpp"


class terrain_component : public entity_component
{
public:

    terrain_component(int grid_x, int grid_z, int size, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap, mat4f* shadowMap, texture* depth_map);
    void operator=(const terrain_component& terrain) {}

    ~terrain_component()
    {
        delete m_heights;
    }

    virtual void init();
    virtual void update(float delta, const camera &cam);
    virtual void render() const;
    virtual void process_input(const input& input, float delta);

    virtual void set_all_uni(camera& cam);

    float barry_centric(const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec2f& pos) const;
    float get_terrain_y(float world_x, float world_z);
private:

    int m_size = 512;
    int m_num_vertices = 4; //256
    bool m_draw = true;
    bool m_draw_line_mode = true;

    int m_grid_x, m_grid_z;
    std::vector<std::vector<float>>* m_heights;

    std::vector<std::vector<terrain_block*>> m_blocks;

    texture* m_background;
    texture* m_r_tex, *m_g_tex, *m_b_tex;
    texture* m_blend_map;
    texture* m_normal_map;

    texture* m_depth_map;
    mat4f* m_shadow_mvp;

    quad_tree m_tree;


    void split_terrain(int num_split, int num_vertices);
    void load_raw_heights(const std::string &path);
    std::vector<std::vector<float>>* load_noise_heights(int num_vertices, unsigned int seed);
    std::vector<std::vector<float>>* load_height_map();
    void connect_seams(terrain_block* block);

    bool is_equal(float a, float b, float epsilon)
    {
        return fabs(a - b) < epsilon;
    }

    void save_image(int w, int h, int channels_num);

    void connect_rest(terrain_block* block, terrain_block* adjacent, mesh** seams, std::string name, bool norm_indices, bool is_vertical,
                                         int a, int b);
};

#endif