#ifndef GAME_ENGINE_CAMERA_H
#define GAME_ENGINE_CAMERA_H

#include "entity_component.hpp"
#include "frustum/frustum.hpp"

class camera
{
public:
	camera(const mat4f& projection, transform* transform, frustum* _frustum) :
            m_projection_matrix(projection),
            m_transformation(transform),
            m_frustum(_frustum)
	{}

	~camera() {}

	
	inline transform* get_transform() { return m_transformation; }
	inline const transform& get_transform() const { return *m_transformation; }
    
    quaternion invert_pitch(quaternion old_rot);
    inline void reverse_pitch(quaternion rot)
    {
        m_transformation->set_rot(rot);
    }

	mat4f get_view_projection() const;
    mat4f get_view_matrix() const;
    mat4f get_projection_matrix() const { return m_projection_matrix; }
    inline void set_cutting_plane(vec4f plane) { m_cutting_plane = plane; }
    inline vec4f get_cutting_plane() const { return m_cutting_plane; }
	
	inline void set_projection(const mat4f &projection) { m_projection_matrix = projection; }
	inline void set_transform(transform *transform) { m_transformation = transform; }

	inline frustum get_frustum() { return *m_frustum; }
    inline const frustum get_frustum() const { return *m_frustum; }
private:
	mat4f m_projection_matrix;
	transform* m_transformation;
	vec4f m_cutting_plane;
	frustum* m_frustum;
};


#endif