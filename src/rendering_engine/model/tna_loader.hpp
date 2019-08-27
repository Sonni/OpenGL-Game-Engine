#ifndef ___D_rendering_engine__tennaLoader__
#define ___D_rendering_engine__tennaLoader__

#include "loader.hpp"
#include "indexed_model.hpp"

class tna_model
{
public:
    std::vector<vec2f> m_uvs;
    std::vector<vec3f> m_vertices;
    std::vector<vec3f> m_normals;
    std::vector<vec3i> m_indices;
    std::vector<vec3i> m_reduced_indices;

    std::vector<float> m_weights;
    std::vector<int> m_joint_ids;

    joint* m_root;

    std::vector<keyframe> m_key_frames;

    float m_animation_len;
    float m_animation_time = 0;
    bool m_has_animation = false;

    tna_model(const std::string& fileName);

    void update(float delta);

    inline void set_time(float time) { m_animation_time = time; }

    std::map<std::string, mat4f> calc_animation();

    void set_joints_pose(std::map<std::string, mat4f> currentPose, joint *_joint, mat4f parentTransform);

    std::vector<keyframe> get_prev_and_next_frame();
    float calculate_progression(keyframe previousFrame, keyframe nextFrame);
    std::map<std::string, mat4f> interpolate_poses(keyframe previousFrame, keyframe nextFrame, float progression);
    joint_transform interpolate(joint_transform frameA, joint_transform frameB, float progression);
    indexed_model to_indexed_model();

private:
    void load_vertices(const std::string &line);
    void load_uvs(const std::string &line);
    void load_normals(const std::string &line);
    void load_faces(const std::string &line);
    void load_reduced_faces(const std::string &line);
    void load_animation_len(const std::string &line);

    int count = 0; //To get the root
    std::string m_file_name;

};

#endif