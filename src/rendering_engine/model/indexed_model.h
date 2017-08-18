#ifndef ___D_rendering_engine__indexedModel__
#define ___D_rendering_engine__indexedModel__

#include <vector>
#include "../../core_engine/math/vector.h"

class indexed_model
{
public:
    indexed_model() {}
    indexed_model(const std::vector<unsigned int> indices, const std::vector<vec3f>& positions, const std::vector<vec2f>& texCoords,
                 const std::vector<vec3f>& normals = std::vector<vec3f>(), const std::vector<vec3f>& tangents = std::vector<vec3f>(), const bool is_animated = false) :
    m_indices(indices),
    m_positions(positions),
    m_texCoords(texCoords),
    m_normals(normals),
    m_tangents(tangents),
    is_animated(is_animated){}
    
    bool is_valid() const;
    void calc_normals();
    void calc_tangents();
    
    indexed_model finalize();
    
    void add_vertex(const vec3f &vert);
    inline void add_vertex(float x, float y, float z) { add_vertex(vec3f(x, y, z)); }
    
    void add_tex_coord(const vec2f &texCoord);
    inline void add_tex_coord(float x, float y) { add_tex_coord(vec2f(x, y)); }
    
    void add_normal(const vec3f &normal);
    inline void add_normal(float x, float y, float z) { add_normal(vec3f(x, y, z)); }
    
    void add_tangent(const vec3f &tangent);
    inline void add_tangent(float x, float y, float z) { add_tangent(vec3f(x, y, z)); }
    
    inline void add_bone_id(const vec4i &ids) { m_boneIds.push_back(ids); }

    inline void add_weight(const vec4f &weights) { m_weights.push_back(weights); }
    
    void add_face(unsigned int vertIndex0, unsigned int vertIndex1, unsigned int vertIndex2);
    void add_indice(const int indice);
    
    inline const std::vector<unsigned int>& get_indices() const { return m_indices; }
    inline const std::vector<vec3f>& get_positions()   const { return m_positions; }
    inline const std::vector<vec2f>& get_tex_coords()   const { return m_texCoords; }
    inline std::vector<vec2f>& get_tex_coords()    { return m_texCoords; }
    inline const std::vector<vec3f>& get_normals()     const { return m_normals; }
    inline  std::vector<vec3f>& get_normals()      { return m_normals; }
    inline const std::vector<vec3f>& get_tangents()    const { return m_tangents; }
    
    inline const std::vector<vec4i>& get_bone_ids()    const { return m_boneIds; }
    inline const std::vector<vec4f>& get_weights()    const { return m_weights; }

    inline void set_is_animated(const bool is_animated) {this->is_animated = is_animated;}
    inline bool has_animation() const { return is_animated; }

private:
    std::vector<unsigned int> m_indices;
    std::vector<vec3f> m_positions;
    std::vector<vec2f> m_texCoords;
    std::vector<vec3f> m_normals;
    std::vector<vec3f> m_tangents;
    
    std::vector<vec4i> m_boneIds;
    std::vector<vec4f> m_weights;
    bool is_animated;
};

#endif