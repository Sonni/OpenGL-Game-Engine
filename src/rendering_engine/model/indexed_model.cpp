#include "indexed_model.h"

bool indexed_model::is_valid() const
{
    return m_positions.size() == m_texCoords.size()
    && m_texCoords.size() == m_normals.size()
    && m_normals.size() == m_tangents.size();
}

void indexed_model::add_vertex(const vec3f &vert)
{
    m_positions.push_back(vert);
}

void indexed_model::add_tex_coord(const vec2f &texCoord)
{
    m_texCoords.push_back(texCoord);
}

void indexed_model::add_normal(const vec3f &normal)
{
    m_normals.push_back(normal);
}

void indexed_model::add_tangent(const vec3f &tangent)
{
    m_tangents.push_back(tangent);
}

void indexed_model::add_indice(const int indice)
{
    m_indices.push_back(indice);
}

indexed_model indexed_model::finalize()
{
    if(is_valid())
    {
        return *this;
    }
    
    if(m_texCoords.size() == 0)
    {
        for(unsigned int i = (unsigned int) m_texCoords.size(); i < m_positions.size(); i++)
        {
            m_texCoords.push_back(vec2f(0.0f, 0.0f));
        }
    }
    
    if(m_normals.size() == 0)
    {
        calc_normals();
    }
    
    if(m_tangents.size() == 0)
    {
        calc_tangents();
    }
    
    return *this;
}

void indexed_model::add_face(unsigned int vertIndex0, unsigned int vertIndex1, unsigned int vertIndex2)
{
    m_indices.push_back(vertIndex0);
    m_indices.push_back(vertIndex1);
    m_indices.push_back(vertIndex2);
}

void indexed_model::calc_normals()
{
    m_normals.clear();
    m_normals.reserve(m_positions.size());
    
    for(unsigned int i = 0; i < m_positions.size(); i++)
        m_normals.push_back(vec3f(0,0,0));
    
    for(unsigned int i = 0; i < m_indices.size(); i += 3)
    {
        int i0 = m_indices[i];
        int i1 = m_indices[i + 1];
        int i2 = m_indices[i + 2];
        
        vec3f v1 = m_positions[i1] - m_positions[i0];
        vec3f v2 = m_positions[i2] - m_positions[i0];
        
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
    m_tangents.clear();
    m_tangents.reserve(m_positions.size());
    
    for(unsigned int i = 0; i < m_positions.size(); i++)
        m_tangents.push_back(vec3f(0,0,0));
    
    for(unsigned int i = 0; i < m_indices.size(); i += 3)
    {
        int i0 = m_indices[i];
        int i1 = m_indices[i + 1];
        int i2 = m_indices[i + 2];
        
        vec3f edge1 = m_positions[i1] - m_positions[i0];
        vec3f edge2 = m_positions[i2] - m_positions[i0];
        
        float deltaU1 = m_texCoords[i1].get_x() - m_texCoords[i0].get_x();
        float deltaU2 = m_texCoords[i2].get_x() - m_texCoords[i0].get_x();
        float deltaV1 = m_texCoords[i1].get_y() - m_texCoords[i0].get_y();
        float deltaV2 = m_texCoords[i2].get_y() - m_texCoords[i0].get_y();
        
        float dividend = (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        float f = dividend == 0.0f ? 0.0f : 1.0f/dividend;
        
        vec3f tangent = vec3f(0,0,0);

        tangent.set_x(f * (deltaV2 * edge1.get_x() - deltaV1 * edge2.get_x()));
        tangent.set_y(f * (deltaV2 * edge1.get_y() - deltaV1 * edge2.get_y()));
        tangent.set_z(f * (deltaV2 * edge1.get_z() - deltaV1 * edge2.get_z()));
        
        m_tangents[i0] += tangent;
        m_tangents[i1] += tangent;
        m_tangents[i2] += tangent;	
    }
    
    for(unsigned int i = 0; i < m_tangents.size(); i++)
        m_tangents[i] = m_tangents[i].normalized();
}