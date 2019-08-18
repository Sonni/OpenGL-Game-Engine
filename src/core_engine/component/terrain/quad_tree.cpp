#include "quad_tree.h"

quad_tree::quad_tree(std::vector<std::vector<terrain_block*>> blocks) {
    m_root = build_tree(blocks.size() / 2, blocks.size() / 2, blocks.size() / 2, blocks);
    calc_view_box(m_root);
}


quad_node* quad_tree::build_tree(int x, int y, int len_half, std::vector<std::vector<terrain_block*>> blocks)
{
    quad_node* node = new quad_node();

    if (len_half == 0) {
        node->m_block = blocks[x][y];
        node->m_view_box = node->m_block->m_view_box;
        node->m_max_y = node->m_block->m_max_y;
        node->m_min_y = node->m_block->m_min_y;
        return node;
    }

    int new_len_half = ceil(len_half/2);
    int new_len_half2 = ceil((float)len_half/2);

    node->m_top_left = build_tree(x - new_len_half2, y + new_len_half, new_len_half, blocks);
    node->m_top_right = build_tree(x + new_len_half, y + new_len_half, new_len_half, blocks);
    node->m_bottom_left = build_tree(x - new_len_half2, y - new_len_half2, new_len_half, blocks);
    node->m_bottom_right = build_tree(x + new_len_half, y - new_len_half2, new_len_half, blocks);

    return node;
}

void quad_tree::calc_view_box(quad_node* root)
{
    if (root->m_block != nullptr) return;

    calc_view_box(root->m_top_left);
    calc_view_box(root->m_top_right);
    calc_view_box(root->m_bottom_left);
    calc_view_box(root->m_bottom_right);

    float min_y = root->m_top_right->m_min_y, max_y = root->m_top_right->m_max_y;

    if (root->m_top_left->m_min_y < min_y) min_y = root->m_top_left->m_min_y;
    if (root->m_bottom_left->m_min_y < min_y) min_y = root->m_bottom_left->m_min_y;
    if (root->m_bottom_right->m_min_y < min_y) min_y = root->m_bottom_right->m_min_y;

    if (root->m_top_left->m_max_y < min_y) min_y = root->m_top_left->m_max_y;
    if (root->m_bottom_left->m_max_y < min_y) min_y = root->m_bottom_left->m_max_y;
    if (root->m_bottom_right->m_max_y < min_y) min_y = root->m_bottom_right->m_max_y;

    if (root->m_top_left->m_max_y > max_y) max_y = root->m_top_left->m_max_y;
    if (root->m_bottom_left->m_max_y > max_y) max_y = root->m_bottom_left->m_max_y;
    if (root->m_bottom_right->m_max_y > max_y) max_y = root->m_bottom_right->m_max_y;

    if (root->m_top_left->m_min_y > max_y) max_y = root->m_top_left->m_min_y;
    if (root->m_bottom_left->m_min_y > max_y) max_y = root->m_bottom_left->m_min_y;
    if (root->m_bottom_right->m_min_y > max_y) max_y = root->m_bottom_right->m_min_y;


    aabb view_box;
    view_box.set_min_extents(vec3f(root->m_bottom_left->m_view_box.get_min_extents().get_x(), min_y, root->m_bottom_left->m_view_box.get_min_extents().get_z()));
    view_box.set_max_extents(vec3f(root->m_top_right->m_view_box.get_max_extents().get_x(), max_y, root->m_top_right->m_view_box.get_max_extents().get_z()));
    root->m_view_box = view_box;
    root->m_min_y = min_y;
    root->m_max_y = max_y;
}

void quad_tree::find_visible_blocks(const camera& cam) {
    find_visible_blocks(m_root, cam);
}

void quad_tree::find_visible_blocks(quad_node* root, const camera& cam) {
    if (cam.get_frustum().aabb_in_frustum(root->m_view_box) == frustum::INSIDE) {
        if (root->m_block != nullptr)
        {
            root->m_block->m_in_frustum = true;
            return;
        }
        find_visible_blocks(root->m_bottom_left, cam);
        find_visible_blocks(root->m_bottom_right, cam);
        find_visible_blocks(root->m_top_left, cam);
        find_visible_blocks(root->m_top_right, cam);
    }
}