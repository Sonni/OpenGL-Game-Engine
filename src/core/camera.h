#ifndef GAME_ENGINE_CAMERA_H
#define GAME_ENGINE_CAMERA_H

#include "entity_component.h"

class camera
{
public:
	camera(const mat4f& projection, transform* transform) :
			projection_matrix(projection),
			transformation(transform)
	{}

	~camera() {}

	
	inline transform* get_transform() { return transformation; }
	inline const transform& get_transform() const { return *transformation; }
    
    quaternion invert_pitch(quaternion old_rot);
    inline void reverse_pitch(quaternion rot)
    {
		transformation->set_rot(rot);
    }

	mat4f get_view_projection() const;
    mat4f get_view_matrix() const;
    mat4f get_projection_matrix() const { return projection_matrix; }
    inline void set_cutting_plane(vec4f plane) {cutting_plane = plane;}
    inline vec4f get_cutting_plane() const { return cutting_plane; }
	
	inline void set_projection(const mat4f &projection) { this->projection_matrix = projection; }
	inline void set_transform(transform *transform) { transformation = transform; }
private:
	mat4f   projection_matrix;
	transform* transformation;
	vec4f cutting_plane;
};


#endif