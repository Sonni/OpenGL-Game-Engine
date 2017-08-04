#ifndef ___D_rendering_engine__terrain__
#define ___D_rendering_engine__terrain__


#include "../entity_component.h"
#include "../../rendering_engine/texture.h"
#include "../../rendering_engine/model/mesh.h"

class terrain_component : public entity_component
{
public:
    
    terrain_component(mat4f* shadowMap, texture* depth_map, int gridX, int gridY, const std::string& background, const std::string& r, const std::string& g, const std::string& b, const std::string& blendmap);
    void operator=(const terrain_component& terrain) {}
    
    ~terrain_component()
    {
        delete _mesh;
    }

    virtual void init();
    virtual void update(float delta);
    virtual void render() const;
    virtual void set_all_uni(camera& cam);
    
    float heights[256][256];
private:
    
    void load_mesh();
    std::vector<texture*> shadows;
    texture* cur;

    int grid_x;
    int grid_y;
    const int SIZE = 250;
    const float MAX_HEIGHT = 35;
    const float MAX_PIXEL_COLOUR = 256;

    mesh* _mesh;

    texture* background;
    texture* rTexture;
    texture* gTexture;
    texture* bTexture;
    texture* blend_map;

    texture* depth_map;
    mat4f* shadow_mvp;

    GLint background_loc, rTexture_loc, gTexture_loc, bTexture_loc, blendmap_loc;
    GLint model_loc, view_loc, view_projection_loc;
    GLint cut_plane_loc;
    GLint shadow_mvp_loc, shadow_tex_loc, shadow_tex_loc2;
};

#endif

/*
 * float barryCentric(vector3f p1, vector3f p2, vector3f p3, vector2f pos) {
        float det = (p2.get_z() - p3.get_z()) * (p1.get_x() - p3.get_x()) + (p3.get_x() - p2.get_x()) * (p1.get_z() - p3.get_z());
        float l1 = ((p2.get_z() - p3.get_z()) * (pos.get_x() - p3.get_x()) + (p3.get_x() - p2.get_x()) * (pos.get_y() - p3.get_z())) / det;
        float l2 = ((p3.get_z() - p1.get_z()) * (pos.get_x() - p3.get_x()) + (p1.get_x() - p3.get_x()) * (pos.get_y() - p3.get_z())) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * p1.get_y() + l2 * p2.get_y() + l3 * p3.get_y();
    }


    float getHeightOfTerrain(float worldX, float worldZ)
    {
        float terrainX = worldX - get_transform()->get_pos()->get_x();
        float terrainZ = worldZ - get_transform()->get_pos()->get_y(); // get_y = get_z in this case
        float gridSquareSize = SIZE / ((float) sizeof(heights)/sizeof(heights[0]) - 1);
        int gridX = (int) floor(terrainX / gridSquareSize);
        int gridZ = (int) floor(terrainZ / gridSquareSize);


        if (gridX >=  sizeof(heights)/sizeof(heights[0]) - 1 || gridZ >=  sizeof(heights)/sizeof(heights[0]) - 1 | gridX < 0 || gridZ < 0) {
            return -1;
        }

        float xCoord = fmod(terrainX, gridSquareSize) / gridSquareSize;
        float zCoord = fmod(terrainZ, gridSquareSize) / gridSquareSize;
        float answer;

        if (xCoord <= (1-zCoord))
        {
            answer = barryCentric(vector3f(0, heights[gridX][gridZ], 0), vector3f(1, heights[gridX + 1][gridZ], 0),
                                  vector3f(0, heights[gridX][gridZ + 1], 1), vector2f(xCoord, zCoord));
        }
        else
        {
            answer = barryCentric(vector3f(1, heights[gridX + 1][gridZ], 0), vector3f(1, heights[gridX + 1][gridZ + 1], 1),
                                  vector3f(0, heights[gridX][gridZ + 1], 1), vector2f(xCoord, zCoord));
        }
        return answer;
    }

    float cam_height = getHeightOfTerrain(cam.get_transform()->get_pos()->get_x(), cam.get_transform()->get_pos()->get_z());

    if (cam_height > 0 && cam.get_transform()->get_pos()->get_y() < cam_height)
    cam.get_transform()->get_pos()->set_y(cam_height);
 */