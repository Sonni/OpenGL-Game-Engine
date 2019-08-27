#ifndef INC_3D_ENGINE_MOUSE_H
#define INC_3D_ENGINE_MOUSE_H


#include "../math/vector.hpp"
#include "../math/matrix.hpp"
#include "../camera.hpp"
#include "../io/window.hpp"

class mouse_picker
{
public:

    mouse_picker(mat4f& projection_mat) :
            m_projection_mat(projection_mat)
    { }


    vec3f get_mouse_ray(float mouse_x, float mouse_y, const camera& cam)
    {
        mat4f view_mat = cam.get_view_matrix();

        vec2f normalized_coords = get_normalised_device_coords(mouse_x, window::WIN_HEIGHT - mouse_y, window::WIN_WIDTH, window::WIN_HEIGHT);
        vec4f clip_coords(normalized_coords.get_x(), normalized_coords.get_y(), -1.0f, 1.0f);
        vec4f eye_coords = get_eye_coords(clip_coords);

        vec3f ray = get_world_coords(eye_coords, view_mat);


        return ray;
    }


private:
    mat4f& m_projection_mat;

    vec3f get_world_coords(const vec4f& eye_coords, mat4f& view_mat)
    {
        mat4f inverted_view_mat = view_mat.invert();
        vec4f world_ray = inverted_view_mat.transform(eye_coords);
        vec3f mouse_ray = vec3f(world_ray.get_x(), world_ray.get_y(), world_ray.get_z());
        mouse_ray = mouse_ray.normalized();
        return mouse_ray;
    }


    vec4f get_eye_coords(const vec4f& clip_coords)
    {
        mat4f inverted_projection_mat = m_projection_mat.invert();
        vec4f eye_coords = inverted_projection_mat.transform(clip_coords);
        return vec4f(eye_coords.get_x(), eye_coords.get_y(), -1.0f, 0.0f);
    }


    vec2f get_normalised_device_coords(float mouse_x, float mouse_y, float window_width, float window_height)
    {
        return vec2f((2.0f * mouse_x) / window_width - 1.0f, (2.0f * mouse_y) / window_height - 1.0f);
    }

};


#endif //INC_3D_ENGINE_MOUSE_H
