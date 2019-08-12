#ifndef INC_3D_ENGINE_TERRAIN_BLOCK_H
#define INC_3D_ENGINE_TERRAIN_BLOCK_H

#include "../../../rendering_engine/model/mesh.h"
#include "../../io/input.h"
#include "../../camera.h"

class terrain_block {
public:
    terrain_block(std::vector<std::vector<float>> heights, int size, int grid_x, int grid_z, int num_blocks, int num_vert) : heights(heights), size(size), num_vertices(num_vert), grid_x(grid_x), grid_z(grid_z), num_blocks(num_blocks) {
        for (int i = 0; i < MAX_LOD; i++) {
            meshes.push_back(load_block(i+1));
        }
        selected_mesh = meshes.at(0);
    }

    bool wait = false;
    void process_input(const input& input, float delta) {

        if(input.get_key_down(input::KEY_1))
            selected_mesh = meshes.at(0);
        if(input.get_key_down(input::KEY_2))
            selected_mesh = meshes.at(1);
        if(input.get_key_down(input::KEY_3))
            selected_mesh = meshes.at(2);
        if(input.get_key_down(input::KEY_4))
            selected_mesh = meshes.at(3);

        if(input.get_key_down(input::KEY_K) && !wait) {
            use_lod = !use_lod;
            wait = true;
        }
        if(input.get_key_up(input::KEY_K)) {
            wait = false;
        }

    }

    // Returns true if LOD has been changed
    bool update(const camera &cam) {
        vec3f dist = cam.get_transform().get_pos() - vec3f(grid_x*size, 2.0f, grid_z*size);
        if (use_lod) {
            if (dist.length() < 30.0f) {
                selected_mesh = meshes.at(0);
                lod = 1;
                return true;
            }
            else if (dist.length() < 60.0f) {
                selected_mesh = meshes.at(1);
                lod = 2;
                return true;
            }
            else if (dist.length() < 90.0f) {
                selected_mesh = meshes.at(2);
                lod = 3;
                return true;
            }
            else if (dist.length() < 120.0f) {
                selected_mesh = meshes.at(3);
                lod = 4;
                return true;
            }
        }
        return false;
    }

    void render() const {
        if (render_block) selected_mesh->draw();
        glDisable(GL_CULL_FACE);
        if (left_seams != nullptr) left_seams->draw();
        if (right_seams != nullptr) right_seams->draw();
        if (top_seams != nullptr) top_seams->draw();
        if (bottom_seams != nullptr) bottom_seams->draw();
       // glEnable(GL_CULL_FACE);

    }
    int lod = 1;
    int grid_x, grid_z;
    int size;
    int num_vertices;

    std::vector<std::vector<float>> heights;
    bool use_lod = true;

    // Data necessary for connecting seams
    terrain_block *left = nullptr, *right = nullptr, *bottom = nullptr, *top = nullptr;
    mesh *left_seams = nullptr, *right_seams = nullptr, *bottom_seams = nullptr, *top_seams = nullptr;
    std::vector<std::vector<std::vector<vec3f>>> vertices;

    bool render_block = true;


private:
    int MAX_LOD = 4;
    int num_blocks;
    std::vector<mesh*> meshes;
    mesh* selected_mesh;

    mesh* load_block(const int LOD);
    float get_height(int x, int z);

};


#endif //INC_3D_ENGINE_TERRAIN_BLOCK_H
