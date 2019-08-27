#include "simple_objects.hpp"
#include "../core_engine/math/math.hpp"

simple_object::simple_object(const std::string& name, const GLenum poly_mode) : m_name(name), m_poly_mode(poly_mode) { }

simple_object::~simple_object() { }

void simple_object::render() const {
    glDisable(GL_CULL_FACE);
    //glPolygonMode(GL_FRONT_AND_BACK, m_poly_mode);
    m_mesh->draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

simple_plane::simple_plane(const vec3f& bl, const vec3f& br, const vec3f& tr, const vec3f& tl, const std::string& name) : simple_object(name)
{
    m_mesh = create_mesh(bl, br, tr, tl);
}

void simple_plane::update_corners(const vec3f& bl, const vec3f& br, const vec3f& tr, const vec3f& tl)
{
    delete m_mesh;
    m_mesh = create_mesh(bl, br, tr, tl);
}

mesh* simple_plane::create_mesh(const vec3f& bl, const vec3f& br, const vec3f& tr, const vec3f& tl)
{
    indexed_model model;

    model.add_vertex(bl); model.add_vertex(br);
    model.add_vertex(tr); model.add_vertex(tl);

    model.add_face(0, 1, 2); model.add_face(2, 0, 3);

    return new mesh(m_name, model.finalize());
}


simple_cube::simple_cube(aabb& box, const std::string& name) : simple_object(name) {
    indexed_model model;

    vec3f min = box.get_min_extents();
    vec3f max = box.get_max_extents();

    model.add_vertex(min);
    model.add_vertex(vec3f(min.get_x(), min.get_y(), max.get_z()));
    model.add_vertex(vec3f(min.get_x(), max.get_y(), max.get_z()));
    model.add_vertex(vec3f(min.get_x(), max.get_y(), min.get_z()));

    model.add_vertex(max);
    model.add_vertex(vec3f(max.get_x(), min.get_y(), max.get_z()));
    model.add_vertex(vec3f(max.get_x(), min.get_y(), min.get_z()));
    model.add_vertex(vec3f(max.get_x(), max.get_y(), min.get_z()));


    model.add_face(0, 1, 2); model.add_face(2, 0, 3);
    model.add_face(3, 0, 6); model.add_face(6, 3, 7);
    model.add_face(6, 5, 4); model.add_face(4, 6, 7);
    model.add_face(2, 1, 5); model.add_face(5, 2, 4);
    model.add_face(5, 1, 0); model.add_face(0, 5, 6);
    model.add_face(4, 2, 3); model.add_face(3, 4, 7);

    m_mesh = new mesh(name, model.finalize());
}

simple_sphere::simple_sphere(const vec3f& center, float radius, const std::string& name, int stacks, int slices) : simple_object(name) {
    indexed_model model;

    for (int i = 0; i <= stacks; ++i)
    {
        float V = (float) i / (float) stacks;
        float phi = V * (float)MATH_PI;

        for (int j = 0; j <= slices; ++j)
        {
            float U = (float) j / (float) slices;
            float theta = U * ((float)MATH_PI * 2);

            // use spherical coordinates to calculate the positions
            float x = cos(theta) * sin(phi);
            float y = cos(phi);
            float z = sin(theta) * sin(phi);

            model.add_vertex(vec3f(x, y, z)*radius+center);
        }
    }

    for (int i = 0; i < slices * stacks + slices; ++i)
    {
        model.add_indice(i); model.add_indice(i + slices + 1); model.add_indice(i + slices);
        model.add_indice(i + slices + 1); model.add_indice(i); model.add_indice(i + 1);
    }

    m_mesh = new mesh(name, model.finalize());
}