#include "terrain.hpp"
#include "../../../rendering_engine/stb_image.h"
#include "../../../rendering_engine/shader/shader.hpp"
#include "../../util/util.hpp"
#include <sstream>
#include "../../../utils/logger.hpp"
#include "perlin_noise.hpp"
#include "../../../todo/simple_objects.hpp"

#include "../../../rendering_engine/stb_image_write.h"

// It only works if the terrain is NOT split. So to generate the normal map,
// load the terrain with split=0, save the image and then load it after with
// split = x
void terrain_component::save_image(int w, int h, int channels_num)
{

    unsigned char data[m_blocks.size() * m_blocks.size() * m_blocks[0][0]->m_normals[0].size() * m_blocks[0][0]->m_normals[0].size() * channels_num *3 *3];

    int index = 0;
    for (int i = 0; i < m_blocks.size(); i++)
    {
        for (int l = 0; l < m_blocks[i][0]->m_normals[0].size(); l++)

            {
                for (int j = 0; j < m_blocks.size(); j++)
                {
                    for (int k = 0; k < m_blocks[i][0]->m_normals[0].size(); k++)

                    {
                    float r = m_blocks[i][i]->m_normals[0][k][m_blocks[i][0]->m_normals[0].size()-1-l].get_x();
                    float g = m_blocks[i][j]->m_normals[0][k][m_blocks[i][0]->m_normals[0].size()-1-l].get_y();
                    float b = m_blocks[i][j]->m_normals[0][k][m_blocks[i][0]->m_normals[0].size()-1-l].get_z();
                    int ir = int(127.0 * (r+1));
                    int ig = int(127.0 * (g+1));
                    int ib = int(127.0 * (b+1));

                    data[index++] = ir;
                    data[index++] = ig;
                    data[index++] = ib;
                }
            }
        }
    }

    stbi_write_png("normal_map35.png", m_blocks.size() * m_blocks[0][0]->m_normals[0].size(),m_blocks.size() * m_blocks[0][0]->m_normals[0].size(), channels_num, data, 0);
}

simple_sphere sphere;

terrain_component::terrain_component(int grid_x, int grid_z, int size, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap, mat4f* shadowMap, texture* depth_map) :
        m_grid_x(grid_x), m_grid_z(grid_z), m_size(size),
        m_background(new texture(background)),
        m_r_tex(new texture(r)), m_g_tex(new texture(g)), m_b_tex(new texture(b)),
        m_blend_map(new texture(blendmap)),
        m_shadow_mvp(shadowMap), m_depth_map(depth_map)
{
    m_normal_map = new texture("terrain/normal_map35.png");

    int num_split = 3;
    int num_vertices = 8;

    split_terrain(num_split, num_vertices);
    m_tree = quad_tree(m_blocks);
    sphere = simple_sphere(vec3f(100, 100, 100), 5, "spheres");
}

void terrain_component::split_terrain(int num_split, int num_vertices)
{
    int split = (int) pow(2, num_split);
    int num_verts = (int )pow(2, num_vertices) + split;
    //load_raw_heights("../res/heights2.raw");

    m_heights = load_noise_heights(num_verts, 231);
    //m_heights = load_height_map();
    //num_verts = (*m_heights).size();

    for (int j = 1; j < split; j++)
    {
        for (int i = 0; i < num_verts; i++)
        {
            (*m_heights)[i][num_verts/split*j - 1] = (*m_heights)[i][num_verts/split*j];
            (*m_heights)[num_verts/split*j - 1][i] = (*m_heights)[num_verts/split*j][i];
        }
        (*m_heights)[num_verts/split*j-1][num_verts/split*j-1] = (*m_heights)[num_verts/split*j][num_verts/split*j];
    }


    for (int i = 0; i < split; i++)
    {
        std::vector<terrain_block*> tmp_blocks;
        for (int j = 0; j < split; j++)
        {
            tmp_blocks.push_back(new terrain_block(m_heights, m_size/split, i, j, split, ceil((float)num_verts/split)));
        }
        m_blocks.push_back(tmp_blocks);
    }

    // set adjacent

    for (int i = 0; i < m_blocks.size(); i++) {
        for (int j = 0; j < m_blocks.size(); j++) {
            if (i-1 >= 0) m_blocks[i][j]->m_left = m_blocks[i-1][j];
            if (i+1 < m_blocks.size()) m_blocks[i][j]->m_right = m_blocks[i+1][j];
            if (j+1 < m_blocks.size()) m_blocks[i][j]->m_top = m_blocks[i][j+1];
            if (j-1 >= 0) m_blocks[i][j]->m_bottom = m_blocks[i][j-1];
        }
    }
}

void terrain_component::init()
{
    get_shader()->use_shader();

    get_shader()->add_uniform("model");
    get_shader()->add_uniform("mvp");

    get_shader()->add_uniform("background_tex");
    get_shader()->add_uniform("r_tex");
    get_shader()->add_uniform("g_tex");
    get_shader()->add_uniform("b_tex");
    get_shader()->add_uniform("blend_tex");

    get_shader()->add_uniform("cutting_plane");

    get_shader()->add_uniform("normal_map");
    get_shader()->add_uniform("sun_pos");
}

void terrain_component::set_all_uni(camera& cam)
{
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_mat4f("mvp", cam.get_view_projection() * get_transform()->get_transformation());

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());
    get_shader()->set_uniform_4f("cutting_plane", cam.get_cutting_plane());


    get_shader()->set_uniform_3f("sun_pos", vec3f(10000, 10000, -1000));

    get_shader()->set_sampler_2d("background_tex", m_background, 0);
    get_shader()->set_sampler_2d("r_tex", m_r_tex, 1);
    get_shader()->set_sampler_2d("g_tex", m_g_tex, 2);
    get_shader()->set_sampler_2d("b_tex", m_b_tex, 3);
    get_shader()->set_sampler_2d("blend_tex", m_blend_map, 4);
    get_shader()->set_sampler_2d("normal_map", m_normal_map, 5);
}

bool key_was_up = true;
void terrain_component::process_input(const input& input, float delta)
{
    for (std::vector<terrain_block*> t_blocks : m_blocks)
        for (terrain_block* block : t_blocks)
            block->process_input(input, delta);

    if(input.get_key_down(input::KEY_9) && key_was_up)
    {
        save_image(128, 128, 3);
        m_draw_line_mode = !m_draw_line_mode;
        key_was_up = false;
    }

    if(input.get_key_up(input::KEY_9))
        key_was_up = true;

}

void terrain_component::update(float delta, const camera &cam)
{
    get_transform()->set_pos(vec3f(m_grid_x * m_size, 0, m_grid_z * m_size));

    for (std::vector<terrain_block*> t_blocks : m_blocks)
        for (terrain_block* block : t_blocks)
            if (block->update(cam))
                connect_seams(block);

    for (std::vector<terrain_block*> t_blocks : m_blocks)
        for (terrain_block* block : t_blocks)
            block->m_in_frustum = false;

    m_tree.find_visible_blocks(cam);
}

void terrain_component::render() const
{
    if (m_draw)
    {
        glDisable(GL_CULL_FACE);

        //sphere.render();
        if (m_draw_line_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (std::vector<terrain_block*> t_blocks : m_blocks)
            for (terrain_block* block : t_blocks)
                block->render();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}


void terrain_component::load_raw_heights(const std::string &path)
{
    std::ifstream file(path);

    logger::log_msg("Trying to load terrain raw heights at: ", path);
    if (file.is_open()) {
        std::string line;
        std::vector<float> tmp;
        int num_lines = 0;
        int num_per_line;

        getline(file, line);
        num_per_line = (int) util::split_string(line, ' ').size();
        file.seekg(0, std::ios::beg); //reset

        while (getline(file, line)) {
            num_lines++;
            std::vector<std::string> h = util::split_string(line, ' ');
            if (h.size() != num_per_line)
            {
                logger::log_err("line ", num_lines, ": only contains ", h.size(), " vertices, it should match the number of vertices in the first line");
            }
            for (int j = 0; j < num_per_line; j++) {
                float a = std::stof(h.at(j));
                tmp.push_back(a);
            }
            m_heights->push_back(tmp);
            tmp.clear();
        }
        m_num_vertices = num_per_line;
        file.close();
        if (num_lines != num_per_line)
        {
            logger::log_err("Number of rows and columns must match: row: ", num_lines, " col: ", num_per_line);
        }
    }
    else
    {
        logger::log_err("Unable to load raw heights");
    }
}

std::vector<std::vector<float>>* terrain_component::load_noise_heights(int num_vertices, unsigned int seed)
{
    srand(seed);
    m_num_vertices = num_vertices;
    std::vector<std::vector<float>>* heights = new std::vector<std::vector<float>>();

    perlin_noise pa(m_grid_x, m_grid_z, m_num_vertices, rand());

    for (int i = 0; i < m_num_vertices; i++)
    {
        std::vector<float> tmp;
        for (int j = 0; j < m_num_vertices; j++)
        {
            tmp.push_back(pa.get_height(i, j));
        }
        heights->push_back(tmp);
    }
    return heights;
}

std::vector<std::vector<float>>* terrain_component::load_height_map()
{
    int width, height, numComponents;
    unsigned char* data = stbi_load("../res/textures/terrain/heightmap1.png", &width, &height, &numComponents, 0);

    int MAX_PIXEL_COLOR = 256;
    int MAX_HEIGHT = 30;

    int rgba[height][height];
    int pointer = 0;
    std::vector<std::vector<float>>* heights = new std::vector<std::vector<float>>();


    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < height; j++)
        {
            rgba[i][j] = data[pointer];

            pointer += numComponents;
        }
    }

    for (int i = 0; i < height; i++)
    {
        std::vector<float> tmp;

        for (int j = 0; j < height; j++)
        {
            float cur_height = (float) rgba[height-1-i][j];
            cur_height += MAX_PIXEL_COLOR / 2.0f;
            cur_height /= MAX_PIXEL_COLOR / 2.0f;
            cur_height *= MAX_HEIGHT;

            tmp.push_back(cur_height);
        }
        heights->push_back(tmp);
    }

    return heights;
}

float terrain_component::barry_centric(const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec2f& pos) const
{
    float det = (p2.get_z() - p3.get_z()) * (p1.get_x() - p3.get_x()) + (p3.get_x() - p2.get_x()) * (p1.get_z() - p3.get_z());
    float l1 = ((p2.get_z() - p3.get_z()) * (pos.get_x() - p3.get_x()) + (p3.get_x() - p2.get_x()) * (pos.get_y() - p3.get_z())) / det;
    float l2 = ((p3.get_z() - p1.get_z()) * (pos.get_x() - p3.get_x()) + (p1.get_x() - p3.get_x()) * (pos.get_y() - p3.get_z())) / det;
    float l3 = 1.0f - l1 - l2;
    return l1 * p1.get_y() + l2 * p2.get_y() + l3 * p3.get_y();
}


float terrain_component::get_terrain_y(float world_x, float world_z)
{
    if ((int) floor(world_x / m_size) != m_grid_x || (int) floor(world_z / m_size) != m_grid_z)
        return -100000;


    float terrain_x = world_x - get_transform()->get_pos()->get_x();
    float terrain_z = world_z - get_transform()->get_pos()->get_z(); // get_y = get_z in this case
    float grid_size = m_size / ((float)m_heights->at(0).size() - 1.0f);
    int x_grid = (int) floor(terrain_x / grid_size);
    int z_grid = (int) floor(terrain_z / grid_size);

    float x = fmod(terrain_x, grid_size) / grid_size;
    float z = fmod(terrain_z, grid_size) / grid_size;
    float terrain_y;

    if (x <= (1 - z))
        terrain_y = barry_centric(vec3f(0, (*m_heights)[x_grid][z_grid], 0), vec3f(1, (*m_heights)[x_grid + 1][z_grid], 0), vec3f(0, (*m_heights)[x_grid][z_grid + 1], 1), vec2f(x, z));
    else
        terrain_y = barry_centric(vec3f(1, (*m_heights)[x_grid + 1][z_grid], 0), vec3f(1, (*m_heights)[x_grid + 1][z_grid + 1], 1), vec3f(0, (*m_heights)[x_grid][z_grid + 1], 1), vec2f(x, z));

    return terrain_y;
}

void terrain_component::connect_seams(terrain_block* block)
{
    if (block->m_left != nullptr)
    {
        int a = block->m_left->m_vertices[block->m_left->m_lod - 1].size() - 1;
        connect_rest(block, block->m_left, &block->m_left_seams, block->m_left_seam_str, false, false, a, 0);
    }

    if (block->m_right != nullptr) {
        int b = block->m_vertices[block->m_lod - 1].size() - 1;
        connect_rest(block, block->m_right, &block->m_right_seams, block->m_right_seam_str, true, false, 0, b);
    }

    if (block->m_top != nullptr) {
        int b = block->m_vertices[block->m_lod - 1].size() - 1;
        connect_rest(block, block->m_top, &block->m_top_seams, block->m_top_seam_str, false, true, 0, b);
    }

    if (block->m_bottom != nullptr) {
        int a = block->m_bottom->m_vertices[block->m_bottom->m_lod - 1].size() - 1;
        connect_rest(block, block->m_bottom, &block->m_bottom_seams, block->m_bottom_seam_str, true, true, a, 0);
    }
}


void terrain_component::connect_rest(terrain_block* block, terrain_block* adjacent, mesh** seams, std::string name, bool norm_indices, bool is_vertical, int a, int b)
{
        if (block->m_lod == adjacent->m_lod) {
            if ((*seams)) delete (*seams);
            (*seams) = nullptr;
        }
        else if (block->m_lod > adjacent->m_lod && adjacent->m_in_frustum) {

            indexed_model model;
            int counter = 0;
            float epsilon = 0.01f;
            for (int i = 0; i < block->m_vertices[block->m_lod-1].size()-1; i++)
            {
                if (is_equal(adjacent->m_vertices[adjacent->m_lod - 1][i * 2][a].get_y(), adjacent->m_vertices[adjacent->m_lod - 1][i * 2 + 1][a].get_y(), epsilon)) continue;
                if (is_vertical)
                {
                    model.add_vertex(adjacent->m_vertices[adjacent->m_lod - 1][i * 2][a]);
                    model.add_vertex(adjacent->m_vertices[adjacent->m_lod - 1][i * 2 + 1][a]);
                    model.add_vertex(adjacent->m_vertices[adjacent->m_lod - 1][i * 2 + 2][a]);

                    model.add_vertex(block->m_vertices[block->m_lod - 1][i][b]);
                    model.add_vertex(block->m_vertices[block->m_lod - 1][i + 1][b]);

                    model.add_tex_coord(adjacent->m_tex_coords[adjacent->m_lod - 1][i*2][a]);
                    model.add_tex_coord(adjacent->m_tex_coords[adjacent->m_lod - 1][i *2+ 1][a]);
                    model.add_tex_coord(adjacent->m_tex_coords[adjacent->m_lod - 1][i *2+ 2][a]);

                    model.add_tex_coord(block->m_tex_coords[block->m_lod - 1][i][b]);
                    model.add_tex_coord(block->m_tex_coords[block->m_lod - 1][i + 1][b]);


                    model.add_normal(adjacent->m_normals[adjacent->m_lod - 1][i * 2][a]);
                    model.add_normal(adjacent->m_normals[adjacent->m_lod - 1][i * 2 + 1][a]);
                    model.add_normal(adjacent->m_normals[adjacent->m_lod - 1][i * 2 + 2][a]);

                    model.add_normal(block->m_normals[block->m_lod - 1][i][b]);
                    model.add_normal(block->m_normals[block->m_lod - 1][i + 1][b]);
                }
                else
                {
                    if (is_equal(adjacent->m_vertices[adjacent->m_lod - 1][a][i * 2].get_y(), adjacent->m_vertices[adjacent->m_lod - 1][a][i * 2 + 1].get_y(), epsilon)) continue;

                    model.add_vertex(adjacent->m_vertices[adjacent->m_lod - 1][a][i * 2]);
                    model.add_vertex(adjacent->m_vertices[adjacent->m_lod - 1][a][i * 2 + 1]);
                    model.add_vertex(adjacent->m_vertices[adjacent->m_lod - 1][a][i * 2 + 2]);

                    model.add_vertex(block->m_vertices[block->m_lod - 1][b][i]);
                    model.add_vertex(block->m_vertices[block->m_lod - 1][b][i + 1]);

                    model.add_tex_coord(adjacent->m_tex_coords[adjacent->m_lod - 1][a][i*2]);
                    model.add_tex_coord(adjacent->m_tex_coords[adjacent->m_lod - 1][a][i *2+ 1]);
                    model.add_tex_coord(adjacent->m_tex_coords[adjacent->m_lod - 1][a][i *2+ 2]);

                    model.add_tex_coord(block->m_tex_coords[block->m_lod - 1][b][i]);
                    model.add_tex_coord(block->m_tex_coords[block->m_lod - 1][b][i + 1]);

                    model.add_normal(adjacent->m_normals[adjacent->m_lod - 1][a][i * 2]);
                    model.add_normal(adjacent->m_normals[adjacent->m_lod - 1][a][i * 2 + 1]);
                    model.add_normal(adjacent->m_normals[adjacent->m_lod - 1][a][i * 2 + 2]);

                    model.add_normal(block->m_normals[block->m_lod - 1][b][i]);
                    model.add_normal(block->m_normals[block->m_lod - 1][b][i + 1]);
                }
                counter++;
            }

            if (norm_indices)
            {
                for (int i = 0; i < counter * 5; i += 5) {
                    model.add_indice(i); model.add_indice(i + 3); model.add_indice(i + 1);
                    model.add_indice(i + 3); model.add_indice(i + 1); model.add_indice(i + 4);
                    model.add_indice(i + 1); model.add_indice(i + 4); model.add_indice(i + 2);
                }
            }
            else
            {
                for (int i = 0; i < counter * 5; i += 5) {
                    model.add_indice(i + 1); model.add_indice(i + 3); model.add_indice(i);
                    model.add_indice(i + 4); model.add_indice(i + 1); model.add_indice(i + 3);
                    model.add_indice(i + 2); model.add_indice(i + 4); model.add_indice(i + 1);
                }
            }

            if ((*seams)) delete (*seams);
            (*seams) = new mesh(name, model);
        }
}

/*
 *
 *         get_shader()->add_uniform("view_projection");
        get_shader()->add_uniform("model");

        get_shader()->add_uniform("background_tex");
        get_shader()->add_uniform("r_tex");
        get_shader()->add_uniform("g_tex");
        get_shader()->add_uniform("b_tex");
        get_shader()->add_uniform("blend_tex");

        get_shader()->add_uniform("cutting_plane");

        get_shader()->add_uniform("shadow_mvp");
        get_shader()->add_uniform("shadow_tex");
        get_shader()->add_uniform("shadow_tex2");

        get_shader()->add_uniform("eye_pos");

        get_shader()->add_uniform("ambient_light");



        get_shader()->set_light_loc();



 *        get_shader()->set_uniform_mat4f("view_projection", cam.get_view_projection());
        get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
        get_shader()->set_uniform_mat4f("shadow_mvp", *m_shadow_mvp);

        get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());
        get_shader()->set_uniform_4f("cutting_plane", cam.get_cutting_plane());

        get_shader()->set_uniform_3f("ambient_light", vec3f(0.1, 0.1, 0.1));


        m_background->bind(0);
        get_shader()->set_uniform_1i("background_tex", 0);

        m_r_tex->bind(1);
        get_shader()->set_uniform_1i("r_tex", 1);

        m_g_tex->bind(2);
        get_shader()->set_uniform_1i("g_tex", 2);

        m_b_tex->bind(3);
        get_shader()->set_uniform_1i("b_tex", 3);

        m_blend_map->bind(4);
        get_shader()->set_uniform_1i("blend_tex", 4);

        m_depth_map->bind(5, true);
        get_shader()->set_uniform_1i("shadow_tex", 5);

        m_cur->bind(6);
        get_shader()->set_uniform_1i("shadow_tex2", 6);

        get_shader()->set_light();
 */