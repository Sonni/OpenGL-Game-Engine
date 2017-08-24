#include "terrain.h"
#include "../../../rendering_engine/stb_image.h"
#include "../../../shader/shader.h"
#include "../../util/util.h"
#include <sstream>


terrain_component::terrain_component(mat4f* shadowMap, texture* depth_map, int _grid_x, int _grid_z, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap)
{
    grid_x = _grid_x;
    grid_z = _grid_z;
    //load_raw_heights();
    load_noise_heights();
    load_mesh();
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;

    this->background = new texture(background);
    r_tex = new texture(r);
    g_tex = new texture(g);
    b_tex = new texture(b);
    blend_map = new texture(blendmap);



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

float move_factor = 0;
float cur_tex = 0;
void terrain_component::update(float delta)
{
    get_transform()->set_pos(vec3f(grid_x * SIZE, 0, grid_z * SIZE));

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

void terrain_component::render() const
{


    _mesh->draw();
}

float terrain_component::get_height(int x, int z)
{
    if (x < 0 && z < 0)
        return heights[0][0];
    if (x < 0 && z >= 256)
        return heights[0][255];
    if (x >= 256 && z < 0)
        return heights[255][0];
    if (x >= 256 && z > 256)
        return heights[255][255];

    if (x < 0)
        return heights[0][z];
    if (x >= 256)
        return heights[255][z];
    if (z < 0)
        return heights[x][0];
    if (z >= 256)
        return heights[x][255];

    return heights[x][z];
}

void terrain_component::load_mesh()
{
    indexed_model model;


    int VERTEX_COUNT = 256;
    
    int count = VERTEX_COUNT * VERTEX_COUNT;
    float vertices[count * 3];
    float tex_coords[count * 2];
    int pointer = 0;

    
    for(int i = 0; i < VERTEX_COUNT; i++)
    {
        for(int j = 0; j < VERTEX_COUNT; j++)
        {
            vertices[pointer * 3] = (float) i / ((float) VERTEX_COUNT - 1) * SIZE;
            vertices[pointer * 3 + 1] = heights[i][j];
            vertices[pointer * 3 + 2] = (float) j / ((float) VERTEX_COUNT - 1) * SIZE;



            vec3f n(get_height(i-1, j) - get_height(i+1, j), 2.0f, get_height(i, j-1) - get_height(i, j+1));
            n = n.normalized();

            
            tex_coords[pointer * 2] = (float) i / ((float) VERTEX_COUNT - 1);
            tex_coords[pointer * 2 + 1] = (float) j / ((float) VERTEX_COUNT - 1);
            
            
            model.add_vertex(vec3f(vertices[pointer * 3], vertices[pointer * 3 + 1], vertices[pointer * 3 + 2]));
            model.add_tex_coord(vec2f(tex_coords[pointer * 2], tex_coords[pointer * 2 + 1]));
            model.add_normal(n);

            pointer++;
        }
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
    _mesh = new mesh("Terrain" + std::to_string(grid_x+10000) + std::to_string(grid_z+10000), model.finalize());
}


void terrain_component::load_raw_heights()
{
    int _size = 256;
    std::string path = "../res/heights.raw";
    std::ifstream file;
    file.open(path);

    std::vector<float> tmp;
    std::string line;
    if(file.is_open())
    {
        for (int i = 0; i < _size; i++)
        {
            getline(file, line);
            std::vector<std::string> h = util::split_string(line, ' ');
            for (int j = 0; j < _size; j++)
            {
                float a = std::stof(h.at(j));
                tmp.push_back(a);
            }
            heights.push_back(tmp);
            tmp.clear();
        }
    }
    else
    {
        std::cerr << "Unable to load raw heights" << std::endl;
    }
}

void terrain_component::load_noise_heights()
{
    srand (2342);

    perlin_noise pa(grid_x, grid_z, 256, rand());
    std::vector<float> tmp;

    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 256; j++)
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
        terrain_y = barry_centric(vec3f(0, heights[x_grid][z_grid], 0), vec3f(1, heights[x_grid + 1][z_grid], 0),
                                  vec3f(0, heights[x_grid][z_grid + 1], 1), vec2f(x, z));
    }
    else
    {
        terrain_y = barry_centric(vec3f(1, heights[x_grid + 1][z_grid], 0),
                                  vec3f(1, heights[x_grid + 1][z_grid + 1], 1),
                                  vec3f(0, heights[x_grid][z_grid + 1], 1), vec2f(x, z));
    }
    return terrain_y;
}