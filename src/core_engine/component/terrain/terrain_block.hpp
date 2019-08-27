#ifndef INC_3D_ENGINE_TERRAIN_BLOCK_HPP
#define INC_3D_ENGINE_TERRAIN_BLOCK_HPP

#include "../../../rendering_engine/model/mesh.hpp"
#include "../../io/input.hpp"
#include "../../camera.hpp"

class terrain_block {
public:
    int m_lod = 1;
    int m_grid_x, m_grid_z;
    int m_size;
    int m_num_vertices;
    float m_min_y = INT_MAX, m_max_y = INT_MIN;


    std::vector<std::vector<float>>* m_heights;
    bool m_use_lod = true;

    aabb m_view_box;
    bool m_in_frustum = false;
    bool m_render_block = true;

    // Data necessary for connecting seams
    terrain_block *m_left = nullptr, *m_right = nullptr, *m_bottom = nullptr, *m_top = nullptr;
    mesh *m_left_seams = nullptr, *m_right_seams = nullptr, *m_bottom_seams = nullptr, *m_top_seams = nullptr;
    std::vector<std::vector<std::vector<vec3f>>> m_vertices;
    std::vector<std::vector<std::vector<vec3f>>> m_normals;
    std::vector<std::vector<std::vector<vec2f>>> m_tex_coords;


    std::string m_left_seam_str, m_right_seam_str, m_top_seam_str, m_bottom_seam_str;


    terrain_block(std::vector<std::vector<float>>* heights, int size, int grid_x, int grid_z, int num_blocks, int num_vert);

    // Returns true if LOD has been changed
    bool update(const camera &cam);
    void render() const;
    void process_input(const input& input, float delta);

private:
    int m_MAX_LOD = 4;
    int m_num_blocks;
    std::vector<mesh*> m_meshes;
    mesh* m_selected_mesh;
    float m_lod_start = 100.0f;
    float m_lod_inc = 80.0f;
    bool m_wait = false;


    mesh* load_block(const int LOD);
    float get_height(int x, int z);


};


#endif //INC_3D_ENGINE_TERRAIN_BLOCK_HPP
