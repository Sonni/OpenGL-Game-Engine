#include "terrain.h"
#include "../../rendering_engine/stb_image.h"
#include "../../shader/shader.h"
#include <sstream>


terrain_component::terrain_component(mat4f* shadowMap, texture* depth_map, int gridX, int gridY, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap)
{
    load_mesh();
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;

    this->background = new texture(background);
    rTexture = new texture(r);
    gTexture = new texture(g);
    bTexture = new texture(b);
    blend_map = new texture(blendmap);

    grid_x = gridX;
    grid_y = gridY;

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

    background_loc = get_shader()->get_uni_location("background_tex");
    rTexture_loc = get_shader()->get_uni_location("r_tex");
    gTexture_loc = get_shader()->get_uni_location("g_tex");
    bTexture_loc = get_shader()->get_uni_location("b_tex");
    blendmap_loc = get_shader()->get_uni_location("blend_tex");

    view_projection_loc = get_shader()->get_uni_location("view_projection");
    model_loc = get_shader()->get_uni_location("model");
    view_loc = get_shader()->get_uni_location("view");
    cut_plane_loc = get_shader()->get_uni_location("cutting_plane");

    shadow_mvp_loc = get_shader()->get_uni_location("shadow_mvp");
    shadow_tex_loc = get_shader()->get_uni_location("shadow_tex");
    shadow_tex_loc2 = get_shader()->get_uni_location("shadow_tex2");

    get_shader()->get_light_loc();
}

void terrain_component::set_all_uni(camera& cam)
{
    mat4f worldMatrix = get_transform()->get_transformation();

    glUniformMatrix4fv(view_projection_loc, 1, GL_FALSE, &cam.get_view_projection()[0][0]);
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, &worldMatrix[0][0]);
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, &cam.get_view_matrix()[0][0]);
    glUniformMatrix4fv(shadow_mvp_loc, 1, GL_FALSE, shadow_mvp[0][0]);

    glUniform4f(cut_plane_loc, cam.get_cutting_plane().get_x(), cam.get_cutting_plane().get_y(), cam.get_cutting_plane().get_z(), cam.get_cutting_plane().get_w());

    get_shader()->set_light();


}

float move_factor = 0;
float cur_tex = 0;
void terrain_component::update(float delta) {
    get_transform()->set_pos(vec3f(grid_x * SIZE, 0, grid_y * SIZE));

    move_factor += 5.0f;
    if (move_factor > 20) {
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
   // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    background->bind(0);
    glUniform1i(background_loc, 0);

    rTexture->bind(1);
    glUniform1i(rTexture_loc, 1);

    gTexture->bind(2);
    glUniform1i(gTexture_loc, 2);

    bTexture->bind(3);
    glUniform1i(bTexture_loc, 3);

    blend_map->bind(4);
    glUniform1i(blendmap_loc, 4);

    depth_map->bind(5, true);
    glUniform1i(shadow_tex_loc, 5);

    cur->bind(6);
    glUniform1i(shadow_tex_loc2, 6);

    glDisable(GL_CULL_FACE);
    _mesh->draw();
    glEnable(GL_CULL_FACE);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void terrain_component::load_mesh()
{
    indexed_model model;
   
    int width, height, numComponents;
    unsigned char* data = stbi_load("../res/textures/terrain/heightmap2.png", &width, &height, &numComponents, 4);

    int rgba[height][height];
    int point = 0;

    //Loading red color from heightmap to calculate the the height of each vertex later on.
    for (int i = 0; i < height; i++)
        for (int j = 0; j < height; j++)
        {
            rgba[j][i] = data[point];
            point += numComponents;
        }
    
    int VERTEX_COUNT = height;
    
    int count = VERTEX_COUNT * VERTEX_COUNT;
    float vertices[count * 3];
    float normals[count * 3];
    float textureCoords[count * 2];
    //int indices[6 * (VERTEX_COUNT - 1) * (VERTEX_COUNT * 1)];
    int vertexPointer = 0;
    
    
    std::vector<float> tmp;
    std::vector<std::vector<float>> test;
    
    //Loading Verticies
    for(int i = 0; i < VERTEX_COUNT; i++)
    {
        for(int j = 0; j < VERTEX_COUNT; j++)
        {
            float tmpHeight = rgba[j][i];

            tmpHeight += MAX_PIXEL_COLOUR/2.0;
            tmpHeight /= MAX_PIXEL_COLOUR/2.0;
            tmpHeight *= MAX_HEIGHT;
           
            heights[j][i] = tmpHeight;
            tmp.push_back(tmpHeight);
            
            vertices[vertexPointer * 3] = (float)j / ((float)VERTEX_COUNT - 1) * SIZE;
            vertices[vertexPointer * 3 + 1] = tmpHeight;
            vertices[vertexPointer * 3 + 2] = (float)i / ((float)VERTEX_COUNT - 1) * SIZE;
            
            normals[vertexPointer * 3] = 0;
            normals[vertexPointer * 3 + 1] = 1;
            normals[vertexPointer * 3 + 2] = 0;
            
            textureCoords[vertexPointer * 2] = (float)j / ((float)VERTEX_COUNT - 1);
            textureCoords[vertexPointer * 2 + 1] = (float)i / ((float)VERTEX_COUNT - 1);
            
            
            model.add_vertex(vec3f(vertices[vertexPointer * 3], vertices[vertexPointer * 3 + 1], vertices[vertexPointer * 3 + 2]));
            model.add_tex_coord(vec2f(textureCoords[vertexPointer * 2], textureCoords[vertexPointer * 2 + 1]));
            //model.AddNormal(Vector3f(normals[vertexPointer * 3], normals[vertexPointer * 3 + 1], normals[vertexPointer * 3 + 2]));
            
            vertexPointer++;
        }
        test.push_back(tmp);
        tmp.clear();
    }
    
    for(int gz = 0; gz < VERTEX_COUNT - 1; gz++)
    {
        for(int gx = 0; gx < VERTEX_COUNT - 1; gx++)
        {
            int topLeft = (gz * VERTEX_COUNT) + gx;
            int topRight = topLeft + 1;
            int bottomLeft = ((gz + 1) * VERTEX_COUNT) + gx;
            int bottomRight = bottomLeft + 1;
            
            model.add_indice(topLeft);
            model.add_indice(bottomLeft);
            model.add_indice(topRight);
            model.add_indice(topRight);
            model.add_indice(bottomLeft);
            model.add_indice(bottomRight);
            
        }
    }
    
    model.calc_normals();
    _mesh = new mesh("Terrain", model.finalize());
    stbi_image_free(data);
}