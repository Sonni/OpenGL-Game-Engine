#include "terrain.h"
#include "../../../rendering_engine/stb_image.h"
#include "../../../shader/shader.h"
#include "../../util/util.h"
#include <sstream>
#include "../../../utils/logger.h"
#include "perlin_noise.h"


terrain_component::terrain_component(mat4f* shadowMap, texture* depth_map, int grid_x, int grid_z, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap, const int lod)
{
    this->lod = lod;
    this->grid_x = grid_x;
    this->grid_z = grid_z;
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;

    this->background = new texture(background);
    r_tex = new texture(r);
    g_tex = new texture(g);
    b_tex = new texture(b);
    blend_map = new texture(blendmap);

    setup_viewbox();
    setup_caustic();

    //load_raw_heights("../res/heights2.raw");

    int split = pow(2,3);
    int tmp_size = pow(2,7)+split;
    load_noise_heights(tmp_size);

    for (int j = 1; j < split; j++)
    {
        for (int i = 0; i < tmp_size; i++)
        {
            heights[i][tmp_size/split*j - 1] = heights[i][tmp_size/split*j];
            heights[tmp_size/split*j - 1][i] = heights[tmp_size/split*j][i];
        }
    }

    for (int i = 1; i < split; i++)
    {
        heights[tmp_size/split*i-1][tmp_size/split*i-1] = heights[tmp_size/split*i][tmp_size/split*i];
    }



    for (int i = 0; i < split; i++)
    {
        std::vector<terrain_block*> tmp_blocks;
        for (int j = 0; j < split; j++)
        {
            tmp_blocks.push_back(new terrain_block(heights, SIZE/split, i, j, split, ceil((float)tmp_size/split)));
        }
        blocks.push_back(tmp_blocks);
    }

    // set adjacent

    for (int i = 0; i < blocks.size(); i++) {
        for (int j = 0; j < blocks.size(); j++) {
            if (i-1 >= 0) blocks[i][j]->left = blocks[i-1][j];
            if (i+1 < blocks.size()) blocks[i][j]->right = blocks[i+1][j];
            if (j+1 < blocks.size()) blocks[i][j]->top = blocks[i][j+1];
            if (j-1 >= 0) blocks[i][j]->bottom = blocks[i][j-1];
        }
    }
}

void terrain_component::init() {
    get_shader()->use_shader();

    get_shader()->add_uniform("view_projection");
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
}

void terrain_component::set_all_uni(camera& cam)
{
    get_shader()->set_uniform_mat4f("view_projection", cam.get_view_projection());
    get_shader()->set_uniform_mat4f("model", get_transform()->get_transformation());
    get_shader()->set_uniform_mat4f("shadow_mvp", *shadow_mvp);

    get_shader()->set_uniform_3f("eye_pos", *cam.get_transform()->get_pos());
    get_shader()->set_uniform_4f("cutting_plane", cam.get_cutting_plane());

    get_shader()->set_uniform_3f("ambient_light", vec3f(0.1, 0.1, 0.1));


    background->bind(0);
    get_shader()->set_uniform_1i("background_tex", 0);

    r_tex->bind(1);
    get_shader()->set_uniform_1i("r_tex", 1);

    g_tex->bind(2);
    get_shader()->set_uniform_1i("g_tex", 2);

    b_tex->bind(3);
    get_shader()->set_uniform_1i("b_tex", 3);

    blend_map->bind(4);
    get_shader()->set_uniform_1i("blend_tex", 4);

    depth_map->bind(5, true);
    get_shader()->set_uniform_1i("shadow_tex", 5);

    cur->bind(6);
    get_shader()->set_uniform_1i("shadow_tex2", 6);

    get_shader()->set_light();
}

bool key_was_up = true;
void terrain_component::process_input(const input& input, float delta)
{
    for (std::vector<terrain_block*> t_blocks : blocks)
        for (terrain_block* block : t_blocks)
            block->process_input(input, delta);

    if(input.get_key_down(input::KEY_9) && key_was_up)
    {
        draw_line_mode = !draw_line_mode;
        key_was_up = false;
    }

    if(input.get_key_up(input::KEY_9))
        key_was_up = true;

}

float move_factor = 0;
float cur_tex = 0;
void terrain_component::update(float delta, const camera &cam)
{
    get_transform()->set_pos(vec3f(grid_x * SIZE, 0, grid_z * SIZE));

    for (std::vector<terrain_block*> t_blocks : blocks)
        for (terrain_block* block : t_blocks)
            if (block->update(cam))
                connect_seams(block);

    if (cam.get_frustum().sphere_in_frustum(*_sphere))
        draw = true;
    else
        draw = false;

    draw = true; // TODO
    update_caustic();
}

void terrain_component::render() const
{
    if (draw)
    {
        if (draw_line_mode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (std::vector<terrain_block*> t_blocks : blocks)
            for (terrain_block* block : t_blocks)
                block->render();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}



void terrain_component::load_raw_heights(const std::string &path)
{
    std::vector<float> tmp;
    std::string line;
    int num_lines = 0;
    int num_per_line;



    std::ifstream file(path);

    logger::log_msg("Trying to load terrain raw heights at: ", path);
    if (file.is_open()) {
        std::string line;

        getline(file, line);
        num_per_line = util::split_string(line, ' ').size();
        file.seekg(0, std::ios::beg); //reset

        while (getline(file, line)) {
            num_lines++;
            std::vector<std::string> h = util::split_string(line, ' ');
            if (h.size() != num_per_line)
            {
                std::cerr << "line " << num_lines << ": only contains " << h.size() << " vertices, it should match the number of vertices in the first line" << std::endl;
            }
            for (int j = 0; j < num_per_line; j++) {
                float a = std::stof(h.at(j));
                tmp.push_back(a);
            }
            heights.push_back(tmp);
            tmp.clear();
        }
        num_vertices = num_per_line;
        file.close();
    }
    else
    {
        std::cerr << "Unable to load raw heights" << std::endl;
    }


    if (num_lines != num_per_line)
    {
        std::cerr << "Number of rows and columns must match: row: " << num_lines << " col: " << num_per_line << std::endl;
    }

    logger::log_err("This", " is error ", 432, ".");
    logger::log_warning("This", " is warning ", 231.0, ".");

}

void terrain_component::load_noise_heights(const int vertices)
{
    srand (2342);
    num_vertices = vertices;

    perlin_noise pa(grid_x, grid_z, num_vertices, rand());
    std::vector<float> tmp;

    for (int i = 0; i < num_vertices; i++)
    {
        for (int j = 0; j < num_vertices; j++)
        {
            tmp.push_back(pa.get_height(i, j));
        }
        heights.push_back(tmp);
        tmp.clear();
    }
}

void terrain_component::load_height_map()
{
    int width, height, numComponents;
    unsigned char* data = stbi_load("../res/textures/terrain/heightmap2.png", &width, &height, &numComponents, 4);

    int MAX_PIXEL_COLOR = 256;
    int MAX_HEIGHT = 40;

    int rgba[height][height];
    int pointer = 0;
    std::vector<float> tmp;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < height; j++)
        {
            rgba[j][i] = data[pointer];
            float height = (float) rgba[j][i];
            height += MAX_PIXEL_COLOR / 2.0f;
            height /= MAX_PIXEL_COLOR / 2.0f;
            height *= MAX_HEIGHT;

            tmp.push_back(height);


            pointer += numComponents;
        }
        heights.push_back(tmp);
        tmp.clear();
    }
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
    if ((int) floor(world_x / SIZE) != grid_x || (int) floor(world_z / SIZE) != grid_z)
        return -100000;


    float terrain_x = world_x - get_transform()->get_pos()->get_x();
    float terrain_z = world_z - get_transform()->get_pos()->get_z(); // get_y = get_z in this case
    float grid_size = SIZE / ((float)heights.at(0).size() - 1.0f);
    int x_grid = (int) floor(terrain_x / grid_size);
    int z_grid = (int) floor(terrain_z / grid_size);

    float x = (float) fmod(terrain_x, grid_size) / grid_size;
    float z = (float) fmod(terrain_z, grid_size) / grid_size;
    float terrain_y;

    if (x <= (1 - z))
    {
        terrain_y = barry_centric(vec3f(0, heights[x_grid][z_grid], 0), vec3f(1, heights[x_grid + 1][z_grid], 0), vec3f(0, heights[x_grid][z_grid + 1], 1), vec2f(x, z));
    }
    else
    {
        terrain_y = barry_centric(vec3f(1, heights[x_grid + 1][z_grid], 0), vec3f(1, heights[x_grid + 1][z_grid + 1], 1), vec3f(0, heights[x_grid][z_grid + 1], 1), vec2f(x, z));
    }
    return terrain_y;
}

void terrain_component::setup_viewbox() {
    float mid_y = (max_y + min_y) / 2.0f;
    float mid_x = (grid_x * SIZE + (grid_x + 1) * SIZE) / 2.0f;
    float mid_z = (grid_z * SIZE + (grid_z + 1) * SIZE) / 2.0f;

    vec3f center(mid_x, mid_y, mid_z);

    float radius = fabs(max_y) - fabs(mid_y);

    vec3f start(grid_x * SIZE, mid_y, grid_z * SIZE);

    float start_len = (start-center).length();
    if (start_len > radius)
        radius = start_len;

    _sphere = new sphere(center, radius);
}

void terrain_component::setup_caustic()
{
    for (int i = 0; i < 10; i++)
    {
        std::ostringstream name;
        name << "caustic/CAUST0" << i << ".png";
        shadows.push_back(new texture(name.str()));
    }
    for (int i = 0; i < 10; i++)
    {
        std::ostringstream name;
        name << "caustic/CAUST1" << i << ".png";
        shadows.push_back(new texture(name.str()));
    }
    for (int i = 0; i < 10; i++)
    {
        std::ostringstream name;
        name << "caustic/CAUST2" << i << ".png";
        shadows.push_back(new texture(name.str()));
    }

    for (int i = 0; i < 2; i++)
    {
        std::ostringstream name;
        name << "caustic/CAUST3" << i << ".png";
        shadows.push_back(new texture(name.str()));
    }

    cur = shadows.at(0);
}

void terrain_component::update_caustic()
{
    move_factor += 5.0f;
    if (move_factor > 20)
    {
        cur = shadows.at(cur_tex);
        cur_tex += 1;
        move_factor = 0.0f;
    }
    if (cur_tex > 31)
    {
        cur_tex = 0;
    }
}

int model_counter = 0;
void terrain_component::connect_seams(terrain_block* block)
{
    if (block->left != nullptr)
    {
        if (block->lod == block->left->lod) block->left_seams = nullptr; // memory leak
        else {

            terrain_block *most_detailed, *other;
            if (block->lod > block->left->lod) {
                most_detailed = block->left;
                other = block;
            }
            else {
                most_detailed = block;
                other = block->left;
            }


            indexed_model model;


            int counter = 0;

            for (int i = 0; i < other->vertices[other->lod - 1].size() - 1; i++) {
                int tmp = most_detailed->vertices[most_detailed->lod - 1].size() - 1;
                int tmp2 = other->vertices[other->lod - 1].size() - 1;
                if (most_detailed == block) {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][0][i * 2]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][0][i * 2 + 1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][0][i * 2 + 2]);

                    model.add_vertex(other->vertices[other->lod - 1][tmp2][i]);
                    model.add_vertex(other->vertices[other->lod - 1][tmp2][i + 1]);
                    goto skip3;
                }
                else {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][tmp][i * 2]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][tmp][i * 2 + 1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][tmp][i * 2 + 2]);

                    model.add_vertex(other->vertices[other->lod - 1][0][i]);
                    model.add_vertex(other->vertices[other->lod - 1][0][i + 1]);
                }
                counter++;
            }

            for (int i = 0; i < counter * 5; i += 5) {
                model.add_indice(i);
                model.add_indice(i + 3);
                model.add_indice(i + 1);

                model.add_indice(i + 3);
                model.add_indice(i + 1);
                model.add_indice(i + 4);

                model.add_indice(i + 1);
                model.add_indice(i + 4);
                model.add_indice(i + 2);
            }

            model.calc_normals();
            block->left_seams = new mesh("Seam" + std::to_string(model_counter), model.finalize());
            model_counter++;

        }
    }

    skip3:

    if (block->right != nullptr) {
        if (block->lod == block->right->lod) block->right_seams = nullptr;
        else {

            terrain_block *most_detailed, *other;
            if (block->lod > block->right->lod) {
                most_detailed = block->right;
                other = block;
            }
            else {
                most_detailed = block;
                other = block->right;
            }


            indexed_model model;


            int counter = 0;
            int tmp = most_detailed->vertices[most_detailed->lod - 1].size() - 1;
            int tmp2 = other->vertices[other->lod - 1].size() - 1;
            for (int i = 0; i < other->vertices[other->lod - 1].size() - 1; i++) {

                if (most_detailed == block) {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][tmp][i * 2]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][tmp][i * 2 + 1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][tmp][i * 2 + 2]);

                    model.add_vertex(other->vertices[other->lod - 1][tmp2][i]);
                    model.add_vertex(other->vertices[other->lod - 1][tmp2][i + 1]);
                    goto skip;
                }
                else {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][0][i * 2]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][0][i * 2 + 1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod - 1][0][i * 2 + 2]);

                    model.add_vertex(other->vertices[other->lod - 1][tmp2][i]);
                    model.add_vertex(other->vertices[other->lod - 1][tmp2][i + 1]);

                }
                counter++;
            }

            for (int i = 0; i < counter * 5; i += 5) {
                model.add_indice(i);
                model.add_indice(i + 3);
                model.add_indice(i + 1);

                model.add_indice(i + 3);
                model.add_indice(i + 1);
                model.add_indice(i + 4);

                model.add_indice(i + 1);
                model.add_indice(i + 4);
                model.add_indice(i + 2);
            }

            model.calc_normals();
            block->right_seams = new mesh("Seam" + std::to_string(model_counter), model.finalize());
            model_counter++;

        }
    }
    skip2:

    if (block->top != nullptr) {
        if (block->lod == block->top->lod) block->top_seams = nullptr;
        else {
            terrain_block* most_detailed, *other;
            if (block->lod > block->top->lod) { most_detailed = block->top; other = block; }
            else { most_detailed = block; other = block->top; }


            indexed_model model;


            int counter = 0;
            int tmp = most_detailed->vertices[most_detailed->lod - 1].size() - 1;
            int tmp2 = other->vertices[other->lod - 1].size() - 1;
            for (int i = 0; i < other->vertices[other->lod-1].size()-1; i++)
            {
                if (most_detailed == block)
                {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2][0]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+1][0]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+2][0]);

                    model.add_vertex(other->vertices[other->lod-1][i][tmp2]);
                    model.add_vertex(other->vertices[other->lod-1][i+1][tmp2]);
                    goto skip;
                }
                else
                {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2][0]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+1][0]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+2][0]);

                    model.add_vertex(other->vertices[other->lod-1][i][tmp2]);
                    model.add_vertex(other->vertices[other->lod-1][i+1][tmp2]);
                }
                counter++;
            }

            for (int i = 0; i < counter * 5; i += 5)
            {
                model.add_indice(i);
                model.add_indice(i+3);
                model.add_indice(i+1);

                model.add_indice(i+3);
                model.add_indice(i+1);
                model.add_indice(i+4);

                model.add_indice(i+1);
                model.add_indice(i+4);
                model.add_indice(i+2);
            }

            model.calc_normals();
            block->top_seams = new mesh("Seam" + std::to_string(model_counter), model.finalize());
            model_counter++;
        }
    }
    skip:

    if (block->bottom != nullptr) {
        if (block->lod == block->bottom->lod) block->bottom_seams = nullptr;
        else {
            terrain_block* most_detailed, *other;
            if (block->lod > block->bottom->lod) { most_detailed = block->bottom; other = block; }
            else { most_detailed = block; other = block->bottom; }


            indexed_model model;


            int counter = 0;

            for (int i = 0; i < other->vertices[other->lod-1].size()-1; i++)
            {
                if (most_detailed == block)
                {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2][most_detailed->vertices[most_detailed->lod-1].size()-1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+1][most_detailed->vertices[most_detailed->lod-1].size()-1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+2][most_detailed->vertices[most_detailed->lod-1].size()-1]);

                    model.add_vertex(other->vertices[other->lod-1][i][other->vertices[other->lod-1].size() -1]);
                    model.add_vertex(other->vertices[other->lod-1][i+1][other->vertices[other->lod-1].size() -1]);
                    return;
                }
                else
                {
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2][most_detailed->vertices[most_detailed->lod-1].size()-1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+1][most_detailed->vertices[most_detailed->lod-1].size()-1]);
                    model.add_vertex(most_detailed->vertices[most_detailed->lod-1][i*2+2][most_detailed->vertices[most_detailed->lod-1].size()-1]);

                    model.add_vertex(other->vertices[other->lod-1][i][0]);
                    model.add_vertex(other->vertices[other->lod-1][i+1][0]);

                }
                counter++;
            }

            for (int i = 0; i < counter * 5; i += 5)
            {
                model.add_indice(i);
                model.add_indice(i+3);
                model.add_indice(i+1);

                model.add_indice(i+3);
                model.add_indice(i+1);
                model.add_indice(i+4);

                model.add_indice(i+1);
                model.add_indice(i+4);
                model.add_indice(i+2);
            }

            model.calc_normals();
            block->bottom_seams = new mesh("Seam" + std::to_string(model_counter), model.finalize());
            model_counter++;
        }
    }
}