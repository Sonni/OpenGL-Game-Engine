#include "terrain_block.h"
#include "../../../utils/logger.h"


mesh* terrain_block::load_block(const int LOD)
{
    if (LOD < 1) {
        logger::log_err("terrain_block: Level of detail cannot be less than 1");
        assert(false);
    }

    std::vector<std::vector<vec3f>> tmp;
    this->vertices.push_back(tmp);

    indexed_model model;

    int VERTEX_COUNT = num_vertices; // LOD must not be 0
    for (int i = 0; i < LOD; i++)
        VERTEX_COUNT = ceil((float)VERTEX_COUNT/2);

    int count = VERTEX_COUNT * VERTEX_COUNT;
    float vertices[count * 3];
    float tex_coords[count * 2];
    int pointer = 0;

    float max_y, min_y = heights[0][0];


    for(int i = 0; i < VERTEX_COUNT; i++)
    {
        std::vector<vec3f> tmp_verts;

        for(int j = 0; j < VERTEX_COUNT; j++)
        {
            vertices[pointer * 3] = (float) i / (float)(VERTEX_COUNT - 1) * size + grid_x * size;

            int tmp_x = i * LOD; int tmp_y = j * LOD;
            if (LOD > 1) {
                tmp_x = (LOD-1)*2*i;
                tmp_y = (LOD-1)*2*j;
            }
            if (i == VERTEX_COUNT - 1) {
                if (j == VERTEX_COUNT - 1) {
                    tmp_x = tmp_y = num_vertices-1;
                } else {
                    tmp_x = num_vertices-1;
                }
            } else {
                if (j == VERTEX_COUNT - 1) {
                    tmp_y = num_vertices-1;
                }
            }
            vertices[pointer * 3 + 1] = heights[tmp_x+grid_x*num_vertices][tmp_y+grid_z*num_vertices];
            //vertices[pointer * 3 + 1] = 0;


            vertices[pointer * 3 + 2] = (float) j / (float)(VERTEX_COUNT - 1) * size + grid_z * size;

            if (max_y < heights[i * LOD+grid_x*num_vertices][j * LOD+grid_z*num_vertices])
                max_y = heights[i * LOD+grid_x*num_vertices][j * LOD+grid_z*num_vertices];
            if (min_y > heights[i * LOD+grid_x*num_vertices][j * LOD+grid_z*num_vertices])
                min_y = heights[i * LOD+grid_x*num_vertices][j * LOD+grid_z*num_vertices];



            vec3f n(get_height(tmp_x-1, tmp_y) - get_height(tmp_x+1, tmp_y), 2.0f, get_height(tmp_x, tmp_y-1) - get_height(tmp_x, tmp_y+1));
            n = n.normalized();


            tex_coords[pointer * 2] = ((float)i/num_blocks) / ((float) VERTEX_COUNT - 1) + 1.0f/num_blocks * grid_x;
            tex_coords[pointer * 2 + 1] = ((float)j/num_blocks) / ((float) VERTEX_COUNT - 1) + 1.0f/num_blocks * grid_z;


            model.add_vertex(vec3f(vertices[pointer * 3], vertices[pointer * 3 + 1], vertices[pointer * 3 + 2]));
            model.add_tex_coord(vec2f(tex_coords[pointer * 2], tex_coords[pointer * 2 + 1]));
            model.add_normal(n);

            tmp_verts.push_back(vec3f(vertices[pointer * 3], vertices[pointer * 3 + 1], vertices[pointer * 3 + 2]));

            pointer++;
        }

        this->vertices[this->vertices.size()-1].push_back(tmp_verts);
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


    //model.calc_normals();
    return new mesh("Terrain" + std::to_string(grid_x+10000) + std::to_string(grid_z+10000) + "lod" + std::to_string(LOD), model.finalize());
}

float terrain_block::get_height(int x, int z)
{
    int xm = grid_x*num_vertices;
    int zm = grid_z*num_vertices;
    if (x < 0 && z < 0)
        return heights[xm][zm];
    if (x < 0 && z >= num_vertices)
        return heights[xm][zm+num_vertices - 1];
    if (x >= num_vertices && z < 0)
        return heights[xm+num_vertices - 1][zm];
    if (x >= num_vertices && z > num_vertices)
        return heights[xm+num_vertices - 1][zm+num_vertices - 1];

    if (x < 0)
        if (grid_x != 0)
            return heights[xm-2][zm+z];
        else
            return heights[xm][zm+z];
    if (x >= num_vertices)
        if (xm+num_vertices+1 < heights.size())
            return heights[xm+num_vertices+1][zm+z];
        else
            return heights[xm+num_vertices - 1][zm+z];
    if (z < 0)
        if (grid_z != 0)
            return heights[xm+x][zm-2];
        else
            return heights[xm+x][zm];
    if (z >= num_vertices)
        if (zm+num_vertices + 1 < heights.size())
            return heights[xm+x][zm+num_vertices + 1];
        else
            return heights[xm+x][zm+num_vertices - 1];

    return heights[xm+x][zm+z];
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