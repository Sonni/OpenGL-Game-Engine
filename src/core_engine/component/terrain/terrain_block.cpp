#include "terrain_block.h"
#include "../../../utils/logger.h"

terrain_block::terrain_block(std::vector<std::vector<float>>* heights, int size, int grid_x, int grid_z, int num_blocks, int num_vert) : m_heights(heights), m_size(size), m_num_vertices(num_vert), m_grid_x(grid_x), m_grid_z(grid_z), m_num_blocks(num_blocks),
    m_left_seam_str("LeftSeam" + std::to_string(grid_x) + "," + std::to_string(grid_z)),
    m_right_seam_str("RightSeam" + std::to_string(grid_x) + "," + std::to_string(grid_z)),
    m_top_seam_str("TopSeam" + std::to_string(grid_x) + "," + std::to_string(grid_z)),
    m_bottom_seam_str("BottomSeam" + std::to_string(grid_x) + "," + std::to_string(grid_z))
{
    for (int i = 0; i < m_MAX_LOD; i++) {
        m_meshes.push_back(load_block(i+1));
    }
    m_selected_mesh = m_meshes.at(0);

    for(int i = 0; i < m_num_vertices; i++) {
        for (int j = 0; j < m_num_vertices; j++) {
            if ((*heights)[i+grid_x*m_num_vertices][j+grid_z*m_num_vertices] < m_min_y) m_min_y = (*heights)[i+grid_x*m_num_vertices][j+grid_z*m_num_vertices];
            else if ((*heights)[i+grid_x*m_num_vertices][j+grid_z*m_num_vertices] > m_max_y) m_max_y = (*heights)[i+grid_x*m_num_vertices][j+grid_z*m_num_vertices];
        }
    }

    m_view_box.set_min_extents(vec3f(grid_x * size, m_min_y, grid_z * size));
    m_view_box.set_max_extents(vec3f(grid_x * size + size, m_max_y, grid_z * size + size));
}

void terrain_block::process_input(const input& input, float delta) {

    if(input.get_key_down(input::KEY_1))
        m_selected_mesh = m_meshes.at(0);
    if(input.get_key_down(input::KEY_2))
        m_selected_mesh = m_meshes.at(1);
    if(input.get_key_down(input::KEY_3))
        m_selected_mesh = m_meshes.at(2);
    if(input.get_key_down(input::KEY_4))
        m_selected_mesh = m_meshes.at(3);

    if(input.get_key_down(input::KEY_K) && !m_wait) {
        m_use_lod = !m_use_lod;
        m_wait = true;
    }
    if(input.get_key_up(input::KEY_K)) {
        m_wait = false;
    }

}


bool terrain_block::update(const camera &cam) {
    if (m_in_frustum)
    {
        vec3f dist = cam.get_transform().get_pos() - vec3f(m_grid_x * m_size + m_size / 2, (m_max_y + m_min_y)/2, m_grid_z * m_size + m_size / 2);
        if (m_use_lod)
        {
            for (int i = 0; i < m_MAX_LOD-1; i++) {
                if (dist.length() < m_lod_start + (i * m_lod_inc)) {
                    m_selected_mesh = m_meshes.at(i);
                    m_lod = i+1;
                    return true;
                }
            }

            m_selected_mesh = m_meshes.at(m_MAX_LOD - 1);
            m_lod = 4;
            return true;
        }
    }
    return false;
}

void terrain_block::render() const {
    if (m_in_frustum) {
        if (m_render_block) m_selected_mesh->draw();
        if (m_left_seams != nullptr) m_left_seams->draw();
        if (m_right_seams != nullptr) m_right_seams->draw();
        if (m_top_seams != nullptr) m_top_seams->draw();
        if (m_bottom_seams != nullptr) m_bottom_seams->draw();
    }
}

mesh* terrain_block::load_block(const int LOD)
{
    if (LOD < 1) {
        logger::log_err("terrain_block: Level of detail cannot be less than 1");
        assert(false);
    }

    std::vector<std::vector<vec3f>> tmp;
    m_vertices.push_back(tmp);
    std::vector<std::vector<vec3f>> tmp3;
    m_normals.push_back(tmp3);
    std::vector<std::vector<vec2f>> tmp2;
    m_tex_coords.push_back(tmp2);

    indexed_model model;

    int VERTEX_COUNT = m_num_vertices; // LOD must not be 0
    for (int i = 0; i < LOD-1; i++)
        VERTEX_COUNT = ceil((float)VERTEX_COUNT/2);

    int count = VERTEX_COUNT * VERTEX_COUNT;
    float vertices[count * 3];
    float tex_coords[count * 2];
    int pointer = 0;

    float max_y, min_y = (*m_heights)[0][0];

    for(int i = 0; i < VERTEX_COUNT; i++)
    {
        std::vector<vec3f> tmp_verts;
        std::vector<vec3f> tmp_normals;
        std::vector<vec2f> tmp_tex_coords;

        for(int j = 0; j < VERTEX_COUNT; j++)
        {
            vertices[pointer * 3] = (float) i / (float)(VERTEX_COUNT - 1) * m_size + m_grid_x * m_size;

            int tmp_x = pow(2, LOD-1)*i; int tmp_y = pow(2, LOD-1)*j;

            if (i == VERTEX_COUNT - 1) {
                if (j == VERTEX_COUNT - 1) {
                    tmp_x = tmp_y = m_num_vertices-1;
                } else {
                    tmp_x = m_num_vertices-1;
                }
            } else {
                if (j == VERTEX_COUNT - 1) {
                    tmp_y = m_num_vertices-1;
                }
            }
            vertices[pointer * 3 + 1] = (*m_heights)[tmp_x+m_grid_x*m_num_vertices][tmp_y+m_grid_z*m_num_vertices];
            //vertices[pointer * 3 + 1] = 0;


            vertices[pointer * 3 + 2] = (float) j / (float)(VERTEX_COUNT - 1) * m_size + m_grid_z * m_size;

            if (max_y < (*m_heights)[i * LOD+m_grid_x*m_num_vertices][j * LOD+m_grid_z*m_num_vertices])
                max_y = (*m_heights)[i * LOD+m_grid_x*m_num_vertices][j * LOD+m_grid_z*m_num_vertices];
            if (min_y > (*m_heights)[i * LOD+m_grid_x*m_num_vertices][j * LOD+m_grid_z*m_num_vertices])
                min_y = (*m_heights)[i * LOD+m_grid_x*m_num_vertices][j * LOD+m_grid_z*m_num_vertices];


            int adj = 1;
            vec3f n(get_height(tmp_x-adj, tmp_y) - get_height(tmp_x+adj, tmp_y), 2.0f, get_height(tmp_x, tmp_y-adj) - get_height(tmp_x, tmp_y+adj));
            //n = vec3f(0.0, 1.0, 0.0);
            n = n.normalized();


            tex_coords[pointer * 2] = ((float)i/m_num_blocks) / ((float) VERTEX_COUNT - 1) + 1.0f/m_num_blocks * m_grid_x;
            tex_coords[pointer * 2 + 1] = ((float)j/m_num_blocks) / ((float) VERTEX_COUNT - 1) + 1.0f/m_num_blocks * m_grid_z;


            model.add_vertex(vec3f(vertices[pointer * 3], vertices[pointer * 3 + 1], vertices[pointer * 3 + 2]));
            model.add_tex_coord(vec2f(tex_coords[pointer * 2], tex_coords[pointer * 2 + 1]));
            model.add_normal(n);

            tmp_verts.push_back(vec3f(vertices[pointer * 3], vertices[pointer * 3 + 1], vertices[pointer * 3 + 2]));
            tmp_normals.push_back(vec3f(n.get_x(), n.get_y(), n.get_z()));
            tmp_tex_coords.push_back(vec2f(tex_coords[pointer * 2], tex_coords[pointer * 2 + 1]));
            pointer++;
        }

        m_vertices[m_vertices.size()-1].push_back(tmp_verts);
        m_normals[m_normals.size()-1].push_back(tmp_normals);
        m_tex_coords[m_tex_coords.size()-1].push_back(tmp_tex_coords);
    }

    for(int i = 0; i < VERTEX_COUNT - 1; i++)
    {
        for(int j = 0; j < VERTEX_COUNT - 1; j++)
        {
            int topLeft = (i * VERTEX_COUNT) + j;
            int topRight = topLeft + 1;
            int bottomLeft = ((i + 1) * VERTEX_COUNT) + j;
            int bottomRight = bottomLeft + 1;

            model.add_indice(topLeft);
            model.add_indice(bottomLeft);
            model.add_indice(topRight);
            model.add_indice(topRight);
            model.add_indice(bottomLeft);
            model.add_indice(bottomRight);

        }
    }


    model.calc_tangents();
    return new mesh("Terrain" + std::to_string(m_grid_x+10000) + std::to_string(m_grid_z+10000) + "lod" + std::to_string(LOD), model.finalize());
}

float terrain_block::get_height(int x, int z)
{
    int xm = m_grid_x*m_num_vertices;
    int zm = m_grid_z*m_num_vertices;
    if (x < 0 && z < 0)
        return (*m_heights)[xm][zm];
    if (x < 0 && z >= m_num_vertices)
        return (*m_heights)[xm][zm+m_num_vertices - 1];
    if (x >= m_num_vertices && z < 0)
        return (*m_heights)[xm+m_num_vertices - 1][zm];
    if (x >= m_num_vertices && z > m_num_vertices)
        return (*m_heights)[xm+m_num_vertices - 1][zm+m_num_vertices - 1];

    if (x < 0)
        if (m_grid_x != 0)
            return (*m_heights)[xm-2][zm+z];
        else
            return (*m_heights)[xm][zm+z];
    if (x >= m_num_vertices)
        if (xm+m_num_vertices+1 < m_heights->size())
            return (*m_heights)[xm+m_num_vertices+1][zm+z];
        else
            return (*m_heights)[xm+m_num_vertices - 1][zm+z];
    if (z < 0)
        if (m_grid_z != 0)
            return (*m_heights)[xm+x][zm-2];
        else
            return (*m_heights)[xm+x][zm];
    if (z >= m_num_vertices)
        if (zm+m_num_vertices + 1 < m_heights->size())
            return (*m_heights)[xm+x][zm+m_num_vertices + 1];
        else
            return (*m_heights)[xm+x][zm+m_num_vertices - 1];

    return (*m_heights)[xm+x][zm+z];
}


/*
 * float terrain_block::get_height(int x, int z)
{
    if (x < 0 && z < 0)
        return (*ori_heights)[0][0];
    if (x < 0 && z >= num_vertices)
        return (*ori_heights)[0][num_vertices - 1];
    if (x >= num_vertices && z < 0)
        return (*ori_heights)[num_vertices - 1][0];
    if (x >= num_vertices && z > num_vertices)
        return (*ori_heights)[num_vertices - 1][num_vertices - 1];

    if (x < 0)
        return (*ori_heights)[0][z];
    if (x >= num_vertices)
        return (*ori_heights)[num_vertices - 1][z];
    if (z < 0)
        return (*ori_heights)[x][0];
    if (z >= num_vertices)
        return (*ori_heights)[x][num_vertices - 1];

    return (*ori_heights)[x][z];
}
 */