#ifndef INC_3D_ENGINE_SIMPLE_OBJECTS_HPP
#define INC_3D_ENGINE_SIMPLE_OBJECTS_HPP


#include "../rendering_engine/model/mesh.hpp"
#include "../physics_engine/collider/aabb.hpp"

class simple_object {
public:
    virtual void render() const;

protected:
    mesh* m_mesh = nullptr;
    std::string m_name;
    GLenum m_poly_mode;

    simple_object(const std::string& name, const GLenum poly_mode = GL_LINE);
    ~simple_object();
};

class simple_plane : public simple_object {
public:
    simple_plane(const vec3f& bl, const vec3f& br, const vec3f& tr, const vec3f& tl, const std::string& name);

    void update_corners(const vec3f& bl, const vec3f& br, const vec3f& tr, const vec3f& tl);
private:
    mesh* create_mesh(const vec3f& bl, const vec3f& br, const vec3f& tr, const vec3f& tl);
};

class simple_cube : public simple_object {
public:
    simple_cube(aabb& box, const std::string& name);
    simple_cube() : simple_object("") { m_mesh = nullptr; }

private:

};

class simple_sphere : public simple_object {
public:
    simple_sphere(const vec3f& center, float radius, const std::string& name, int stacks = 20, int slices = 20);
    simple_sphere() : simple_object("") { }

private:

};

class simple_plane2 : public simple_object {
public:
    simple_plane2(const vec3f& start, int width, int breath, int num_verts, const std::string& name) : simple_object(name)
    {
        indexed_model model;


        int VERTEX_COUNT = num_verts;

        int count = VERTEX_COUNT * VERTEX_COUNT;
        float vertices[count * 3];
        float tex_coords[count * 2];
        int pointer = 0;

        for(int i = 0; i < VERTEX_COUNT; i++)
        {
            for(int j = 0; j < VERTEX_COUNT; j++)
            {
                vertices[pointer * 3] = (float) i / ((float) VERTEX_COUNT - 1) * width;
                vertices[pointer * 3 + 1] = 0;
                vertices[pointer * 3 + 2] = (float) j / ((float) VERTEX_COUNT - 1) * breath;


                tex_coords[pointer * 2] = (float) i / ((float) VERTEX_COUNT - 1);
                tex_coords[pointer * 2 + 1] = (float) j / ((float) VERTEX_COUNT - 1);


                model.add_vertex(vec3f(vertices[pointer * 3], vertices[pointer * 3 + 1], vertices[pointer * 3 + 2]));
                model.add_tex_coord(vec2f(tex_coords[pointer * 2], tex_coords[pointer * 2 + 1]));

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

        m_mesh = new mesh(m_name, model.finalize());
    }
    simple_plane2() : simple_object("") { }

private:

};


#endif //INC_3D_ENGINE_SIMPLE_OBJECTS_HPP
