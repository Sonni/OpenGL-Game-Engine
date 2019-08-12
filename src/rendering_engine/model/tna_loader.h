#ifndef ___D_rendering_engine__tennaLoader__
#define ___D_rendering_engine__tennaLoader__

#include "loader.h"
#include "indexed_model.h"

class tna_model
{
public:
    std::vector<vec3f> vertices;
    std::vector<vec2f> uvs;
    std::vector<vec3f> normals;
    std::vector<vec3i> indices;
    std::vector<vec3i> reduced_indices;


    joint* m_root;

    std::vector<keyframe> keyframes;

    float animation_len;


    std::vector<float> m_weights;
    std::vector<int> joint_ids;
    float animationTime = 0;


    tna_model(const std::string& fileName);

    bool has_animation = false;

    void update(float delta)
    {
        animationTime += delta;
        if (animationTime > animation_len)
        {
            animationTime = (float) fmod(animationTime, animation_len);
        }

        set_joints_pose(calc_animation(), m_root, mat4f().init_identity());
    }

    inline void set_time(float time)
    {
        animationTime = time;
    }



    std::map<std::string, mat4f> calc_animation()
    {
        std::vector<keyframe> frames = get_prev_and_next_frame();
        return interpolate_poses(frames[0], frames[1], calculate_progression(frames[0], frames[1]));
    }

    void set_joints_pose(std::map<std::string, mat4f> currentPose, joint *_joint, mat4f parentTransform)
    {
        mat4f currentLocalTransform = currentPose[_joint->name];
        mat4f currentTransform = parentTransform * currentLocalTransform;

        for (joint* childJoint : _joint->children)
        {
            set_joints_pose(currentPose, childJoint, currentTransform);
        }

        currentTransform = currentTransform * _joint->inverse_bind_transform;
        _joint->animated_transform = currentTransform;
    }

    std::vector<keyframe> get_prev_and_next_frame()
    {
        std::vector<keyframe> allFrames = keyframes;
        keyframe previousFrame = allFrames[0];
        keyframe nextFrame = allFrames[0];
        for (int i = 1; i < allFrames.size(); i++)
        {
            nextFrame = allFrames[i];
            if (nextFrame.time_stamp > animationTime)
            {
                break;
            }
            previousFrame = allFrames[i];
        }
        std::vector<keyframe> r;
        r.push_back(previousFrame);
        r.push_back(nextFrame);
        return r;
    }

    float calculate_progression(keyframe previousFrame, keyframe nextFrame)
    {
        float totalTime = nextFrame.time_stamp - previousFrame.time_stamp;
        float currentTime = animationTime - previousFrame.time_stamp;
        return currentTime / totalTime;
    }

    std::map<std::string, mat4f> interpolate_poses(keyframe previousFrame, keyframe nextFrame, float progression)
    {
        std::map<std::string, joint_transform>::iterator it;
        std::map<std::string, mat4f> currentPose;

        for (it = previousFrame.poses->begin(); it != previousFrame.poses->end(); it++)
        {
            joint_transform previousTransform = (*previousFrame.poses)[it->first];
            joint_transform nextTransform = (*nextFrame.poses)[it->first];
            joint_transform currentTransform = interpolate(previousTransform, nextTransform, progression);
            currentPose[it->first] = currentTransform.get_local_transform();
        }

        return currentPose;
    }

    joint_transform interpolate(joint_transform frameA, joint_transform frameB, float progression)
    {
        return joint_transform(vec3f(frameA.get_pos().interpolate(frameB.get_pos(), progression)), frameA.get_rot().interpolate(frameB.get_rot(), progression));
    }

    indexed_model to_indexed_model();
private:
    void load_vertices(const std::string &line);
    void load_uvs(const std::string &line);
    void load_normals(const std::string &line);
    void load_faces(const std::string &line);
    void load_reduced_faces(const std::string &line);
    void load_animation_len(const std::string &line);

    int count = 0; //To get the root
    std::string m_fileName;

};

#endif