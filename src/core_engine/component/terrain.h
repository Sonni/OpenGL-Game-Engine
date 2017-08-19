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

    float barry_centric(const vec3f& p1, const vec3f& p2, const vec3f& p3, const vec2f& pos)
    {
        float det = (p2.get_z() - p3.get_z()) * (p1.get_x() - p3.get_x()) + (p3.get_x() - p2.get_x()) * (p1.get_z() - p3.get_z());
        float l1 = ((p2.get_z() - p3.get_z()) * (pos.get_x() - p3.get_x()) + (p3.get_x() - p2.get_x()) * (pos.get_y() - p3.get_z())) / det;
        float l2 = ((p3.get_z() - p1.get_z()) * (pos.get_x() - p3.get_x()) + (p1.get_x() - p3.get_x()) * (pos.get_y() - p3.get_z())) / det;
        float l3 = 1.0f - l1 - l2;
        return l1 * p1.get_y() + l2 * p2.get_y() + l3 * p3.get_y();
    }


    float get_terrain_y(float world_x, float world_z)
    {
        float terrain_x = world_x - get_transform()->get_pos()->get_x();
        float terrain_z = world_z - get_transform()->get_pos()->get_y(); // get_y = get_z in this case
        float grid_size = SIZE / ((float) sizeof(heights) / sizeof(heights[0]) - 1);
        int grid_x = (int) floor(terrain_x / grid_size);
        int grid_z = (int) floor(terrain_z / grid_size);


        if (grid_x >= sizeof(heights) / sizeof(heights[0]) - 1 || grid_z >=  sizeof(heights) / sizeof(heights[0]) - 1 | grid_x < 0 || grid_z < 0)
        {
            return -1;
        }

        float x = (float) fmod(terrain_x, grid_size) / grid_size;
        float z = (float) fmod(terrain_z, grid_size) / grid_size;
        float terrain_y;

        if (x <= (1 - z))
        {
            terrain_y = barry_centric(vec3f(0, heights[grid_x][grid_z], 0), vec3f(1, heights[grid_x + 1][grid_z], 0),
                                      vec3f(0, heights[grid_x][grid_z + 1], 1), vec2f(x, z));
        }
        else
        {
            terrain_y = barry_centric(vec3f(1, heights[grid_x + 1][grid_z], 0),
                                      vec3f(1, heights[grid_x + 1][grid_z + 1], 1),
                                      vec3f(0, heights[grid_x][grid_z + 1], 1), vec2f(x, z));
        }
        return terrain_y;
    }
private:

    const int SIZE = 500;
    const float MAX_HEIGHT = 10; //35;
    const float MAX_PIXEL_COLOR = 256;
    std::vector<texture*> shadows;
    texture* cur;

    int grid_x;
    int grid_y;

    mesh* _mesh;

    texture* background;
    texture* r_tex;
    texture* g_tex;
    texture* b_tex;
    texture* blend_map;

    texture* depth_map;
    mat4f* shadow_mvp;

    GLint background_loc, r_tex_loc, g_tex_loc, b_tex_loc, blendmap_loc;
    GLint model_loc, view_loc, view_projection_loc;
    GLint cut_plane_loc;
    GLint shadow_mvp_loc, shadow_tex_loc, shadow_tex_loc2;

    void load_mesh();
};

#endif