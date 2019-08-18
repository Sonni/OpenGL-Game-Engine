#include "indexed_model.h"

bool indexed_model::is_valid() const
{
    return m_pos.size() == m_tex_coords.size()
    && m_tex_coords.size() == m_normals.size()
    && m_normals.size() == m_tagents.size();
}

void indexed_model::add_vertex(const vec3f &vert)
{
    m_pos.push_back(vert);
}

void indexed_model::add_tex_coord(const vec2f &texCoord)
{
    m_tex_coords.push_back(texCoord);
}

void indexed_model::add_normal(const vec3f &normal)
{
    m_normals.push_back(normal);
}

void indexed_model::add_tangent(const vec3f &tangent)
{
    m_tagents.push_back(tangent);
}

void indexed_model::add_indice(const int indice)
{
    m_indices.push_back(indice);
}

indexed_model indexed_model::finalize()
{
    if(is_valid()) return *this;

    
    if(m_tex_coords.size() == 0)
        for(unsigned int i = (unsigned int) m_tex_coords.size(); i < m_pos.size(); i++)
            m_tex_coords.push_back(vec2f(0.0f, 0.0f));

    
    if(m_normals.size() == 0) calc_normals();
    if(m_tagents.size() == 0) calc_tangents();

    return *this;
}

void indexed_model::add_face(unsigned int vertIndex0, unsigned int vertIndex1, unsigned int vertIndex2)
{
    m_indices.push_back(vertIndex0); m_indices.push_back(vertIndex1); m_indices.push_back(vertIndex2);
}

void indexed_model::calc_normals()
{
    m_normals.clear();
    m_normals.reserve(m_pos.size());
    
    for(unsigned int i = 0; i < m_pos.size(); i++)
        m_normals.push_back(vec3f(0,0,0));
    
    for(unsigned int i = 0; i < m_indices.size(); i += 3)
    {
        int i0 = m_indices[i];
        int i1 = m_indices[i + 1];
        int i2 = m_indices[i + 2];
        
        vec3f v1 = m_pos[i1] - m_pos[i0];
        vec3f v2 = m_pos[i2] - m_pos[i0];
        
        vec3f normal = v1.cross(v2).normalized();

        m_normals[i0] = m_normals[i0] + normal;
        m_normals[i1] = m_normals[i1] + normal;
        m_normals[i2] = m_normals[i2] + normal;
    }
    
    for(unsigned int i = 0; i < m_normals.size(); i++)
        m_normals[i] = m_normals[i].normalized();
}

void indexed_model::calc_tangents()
{
    m_tagents.clear();
    m_tagents.reserve(m_pos.size());
    
    for(unsigned int i = 0; i < m_pos.size(); i++)
        m_tagents.push_back(vec3f(0,0,0));
    
    for(unsigned int i = 0; i < m_indices.size(); i += 3)
    {
        int i0 = m_indices[i];
        int i1 = m_indices[i + 1];
        int i2 = m_indices[i + 2];
        
        vec3f edge1 = m_pos[i1] - m_pos[i0];
        vec3f edge2 = m_pos[i2] - m_pos[i0];
        
        float deltaU1 = m_tex_coords[i1].get_x() - m_tex_coords[i0].get_x();
        float deltaU2 = m_tex_coords[i2].get_x() - m_tex_coords[i0].get_x();
        float deltaV1 = m_tex_coords[i1].get_y() - m_tex_coords[i0].get_y();
        float deltaV2 = m_tex_coords[i2].get_y() - m_tex_coords[i0].get_y();
        
        float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        float f = dividend == 0.0f ? 0.0f : 1.0f/dividend;
        
        vec3f tangent = vec3f(0,0,0);

        tangent.set_x(f * (deltaV2 * edge1.get_x() - deltaV1 * edge2.get_x()));
        tangent.set_y(f * (deltaV2 * edge1.get_y() - deltaV1 * edge2.get_y()));
        tangent.set_z(f * (deltaV2 * edge1.get_z() - deltaV1 * edge2.get_z()));

        m_tagents[i0] += tangent;
        m_tagents[i1] += tangent;
        m_tagents[i2] += tangent;
    }
    
    for(unsigned int i = 0; i < m_tagents.size(); i++)
        m_tagents[i] = m_tagents[i].normalized();
}