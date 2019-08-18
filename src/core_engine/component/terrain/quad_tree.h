#ifndef INC_3D_ENGINE_QUAD_TREE_H
#define INC_3D_ENGINE_QUAD_TREE_H

#include "terrain_block.h"

class quad_node {
public:
    quad_node() { }
    quad_node* m_top_left = nullptr, *m_top_right = nullptr, *m_bottom_left = nullptr, *m_bottom_right = nullptr;
    terrain_block* m_block = nullptr;
    aabb m_view_box;
    float m_min_y, m_max_y;
private:
};

class quad_tree {
public:
    quad_node* m_root;

    quad_tree(std::vector<std::vector<terrain_block*>> blocks);
    quad_tree() { }

    void find_visible_blocks(const camera& cam);

private:
    quad_node* build_tree(int x, int y, int len_half, std::vector<std::vector<terrain_block*>> blocks);

    void calc_view_box(quad_node* root);
    void find_visible_blocks(quad_node* root, const camera& cam);
};


#endif //INC_3D_ENGINE_QUAD_TREE_H
