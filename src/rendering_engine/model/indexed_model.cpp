#include "indexed_model.h"

bool indexed_model::is_valid() const
{
    return pos.size() == tex_coords.size()
    && tex_coords.size() == normals.size()
    && normals.size() == tagents.size();
}

void indexed_model::add_vertex(const vec3f &vert)
{
    pos.push_back(vert);
}

void indexed_model::add_tex_coord(const vec2f &texCoord)
{
    tex_coords.push_back(texCoord);
}

void indexed_model::add_normal(const vec3f &normal)
{
    normals.push_back(normal);
}

void indexed_model::add_tangent(const vec3f &tangent)
{
    tagents.push_back(tangent);
}

void indexed_model::add_indice(const int indice)
{
    indices.push_back(indice);
}

indexed_model indexed_model::finalize()
{
    if(is_valid())
    {
        return *this;
    }
    
    if(tex_coords.size() == 0)
    {
        for(unsigned int i = (unsigned int) tex_coords.size(); i < pos.size(); i++)
        {
            tex_coords.push_back(vec2f(0.0f, 0.0f));
        }
    }
    
    if(normals.size() == 0)
    {
        calc_normals();
    }
    
    if(tagents.size() == 0)
    {
        calc_tangents();
    }
    
    return *this;
}

void indexed_model::add_face(unsigned int vertIndex0, unsigned int vertIndex1, unsigned int vertIndex2)
{
    indices.push_back(vertIndex0);
    indices.push_back(vertIndex1);
    indices.push_back(vertIndex2);
}

void indexed_model::calc_normals()
{
    normals.clear();
    normals.reserve(pos.size());
    
    for(unsigned int i = 0; i < pos.size(); i++)
        normals.push_back(vec3f(0,0,0));
    
    for(unsigned int i = 0; i < indices.size(); i += 3)
    {
        int i0 = indices[i];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];
        
        vec3f v1 = pos[i1] - pos[i0];
        vec3f v2 = pos[i2] - pos[i0];
        
        vec3f normal = v1.cross(v2).normalized();
        
        normals[i0] = normals[i0] + normal;
        normals[i1] = normals[i1] + normal;
        normals[i2] = normals[i2] + normal;
    }
    
    for(unsigned int i = 0; i < normals.size(); i++)
        normals[i] = normals[i].normalized();
}

void indexed_model::calc_tangents()
{
    tagents.clear();
    tagents.reserve(pos.size());
    
    for(unsigned int i = 0; i < pos.size(); i++)
        tagents.push_back(vec3f(0,0,0));
    
    for(unsigned int i = 0; i < indices.size(); i += 3)
    {
        int i0 = indices[i];
        int i1 = indices[i + 1];
        int i2 = indices[i + 2];
        
        vec3f edge1 = pos[i1] - pos[i0];
        vec3f edge2 = pos[i2] - pos[i0];
        
        float deltaU1 = tex_coords[i1].get_x() - tex_coords[i0].get_x();
        float deltaU2 = tex_coords[i2].get_x() - tex_coords[i0].get_x();
        float deltaV1 = tex_coords[i1].get_y() - tex_coords[i0].get_y();
        float deltaV2 = tex_coords[i2].get_y() - tex_coords[i0].get_y();
        
        float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        float f = dividend == 0.0f ? 0.0f : 1.0f/dividend;
        
        vec3f tangent = vec3f(0,0,0);

        tangent.set_x(f * (deltaV2 * edge1.get_x() - deltaV1 * edge2.get_x()));
        tangent.set_y(f * (deltaV2 * edge1.get_y() - deltaV1 * edge2.get_y()));
        tangent.set_z(f * (deltaV2 * edge1.get_z() - deltaV1 * edge2.get_z()));
        
        tagents[i0] += tangent;
        tagents[i1] += tangent;
        tagents[i2] += tangent;
    }
    
    for(unsigned int i = 0; i < tagents.size(); i++)
        tagents[i] = tagents[i].normalized();
}