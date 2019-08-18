#include "tna_loader.h"
#include "../../core_engine/util/util.h"
#include <fstream>


tna_model::tna_model(const std::string& file_name)
{
    std::setprecision(9);

    m_file_name = file_name;
    std::string path = "../res/models/" + m_file_name + ".tna";
    std::ifstream file;
    file.open(path);

    m_root = new joint();

    std::string line;
    if(file.is_open())
    {
        while(file.good())
        {
            getline(file, line);
            
            size_t lineLength = line.length();
            
            if(lineLength < 2)
                continue;
            
            const char* lineCStr = line.c_str();
            
            switch(lineCStr[0])
            {
                case 'v':
                    if(lineCStr[1] == 't')
                        load_uvs(line);
                    else if(lineCStr[1] == 'n')
                        load_normals(line);
                    else if(lineCStr[1] == ':' || lineCStr[1] == '\t')
                        load_vertices(line);
                    break;
                case 'f':
                    load_faces(line);
                    break;
                case 'r':
                    load_reduced_faces(line);
                    break;
                case 'l':
                    load_animation_len(line);
                    break;
                case 'w':
                    Loader::load_weights(line, m_weights);
                    break;
                case 'j':
                    if(lineCStr[1] == '_')
                        Loader::load_joint_ids(line, m_joint_ids);
                    if(lineCStr[1] == 'o')
                    {
                        m_has_animation = true;
                        Loader::load_joints(line, m_root);

                    }
                    break;

                case 'k':
                    Loader::load_keyframe(line, m_key_frames);
                    break;
                default: break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << path << std::endl;
    }

    m_root->calc_inverse_bind(mat4f().init_identity());
}

indexed_model tna_model::to_indexed_model()
{
    indexed_model result;
    result.set_is_animated(m_has_animation);


    if (m_has_animation)
    {
        for (int i = 0; i < m_vertices.size(); i++)
        {
            result.add_vertex(vec3f(m_vertices[i]));
            result.add_normal(vec3f(m_normals[i]));
            result.add_tex_coord(vec2f(m_uvs[i]));

            result.add_bone_id(vec3i(m_joint_ids[i * 3], m_joint_ids[i * 3 + 1], m_joint_ids[i * 3 + 2]));
            result.add_weight(vec3f(m_weights[i * 3], m_weights[i * 3 + 1], m_weights[i * 3 + 2]));
        }

        for (int i = 0; i < m_reduced_indices.size(); i++)
        {
            result.add_indice(m_reduced_indices[i].get_x());
            result.add_indice(m_reduced_indices[i].get_y());
            result.add_indice(m_reduced_indices[i].get_z());
        }
    }
    else
    {
        for (int i = 0; i < m_indices.size(); i++)
        {
            result.add_vertex(vec3f(m_vertices[m_indices[i].get_x()]));
            result.add_normal(vec3f(m_normals[m_indices[i].get_y()]));
            result.add_tex_coord(vec2f(m_uvs[m_indices[i].get_z()]));
            result.add_indice(i);
        }
    }


    if (!m_has_animation) result.calc_tangents();

    return result;
}

void tna_model::load_vertices(const std::string &line)
{
    std::string vert_line = line;
    vert_line.erase(0, 3);
    
    std::vector<std::string> vertArray = util::split_string(vert_line, ' ');
    for(int i = 0; i < vertArray.size(); i += 3)
        m_vertices.push_back(vec3f(std::stof(vertArray[i]), std::stof(vertArray[i + 1]), std::stof(vertArray[i + 2])));
}

void tna_model::load_uvs(const std::string &line)
{
    std::string uv_line = line;
    uv_line.erase(0, 4);
    
    std::vector<std::string> uvArray = util::split_string(uv_line, ' ');
    for(int i = 0; i < uvArray.size(); i += 2)
        m_uvs.push_back(vec2f(std::stof(uvArray[i]), std::stof(uvArray[i + 1])));
}

void tna_model::load_normals(const std::string &line)
{
    std::string normale_line = line;
    normale_line.erase(0, 4);
    
    std::vector<std::string> normalArray = util::split_string(normale_line, ' ');
    for(int i = 0; i < normalArray.size(); i += 3)
        m_normals.push_back(vec3f(std::stof(normalArray[i]), std::stof(normalArray[i + 1]), std::stof(normalArray[i + 2])));
}

void tna_model::load_faces(const std::string &line)
{
    std::string faces = line;
    faces.erase(0, 3);

    std::vector<std::string> faceArray = util::split_string(faces, ' ');

    for(int i = 0; i < faceArray.size(); i += 3)
    {
        m_indices.push_back(vec3i(stoi(faceArray[i]), stoi(faceArray[i+1]), stoi(faceArray[i+2])));
    }
}

void tna_model::load_reduced_faces(const std::string &line)
{
    std::string faces = line;
    faces.erase(0, 4);

    std::vector<std::string> faceArray = util::split_string(faces, ' ');

    for(int i = 0; i < faceArray.size(); i += 3)
    {
        m_reduced_indices.push_back(vec3i(stoi(faceArray[i]), stoi(faceArray[i+1]), stoi(faceArray[i+2])));
    }
}

void tna_model::load_animation_len(const std::string &line)
{
    m_animation_len = stof(util::split_string(line, ' ')[1]);
}

void tna_model::update(float delta)
{
    m_animation_time += delta;
    if (m_animation_time > m_animation_len)
    {
        m_animation_time = (float) fmod(m_animation_time, m_animation_len);
    }

    set_joints_pose(calc_animation(), m_root, mat4f().init_identity());
}


std::map<std::string, mat4f> tna_model::calc_animation()
{
    std::vector<keyframe> frames = get_prev_and_next_frame();
    return interpolate_poses(frames[0], frames[1], calculate_progression(frames[0], frames[1]));
}

void tna_model::set_joints_pose(std::map<std::string, mat4f> currentPose, joint *_joint, mat4f parentTransform)
{
    mat4f currentLocalTransform = currentPose[_joint->m_name];
    mat4f currentTransform = parentTransform * currentLocalTransform;

    for (joint* childJoint : _joint->m_children)
    {
        set_joints_pose(currentPose, childJoint, currentTransform);
    }

    currentTransform = currentTransform * _joint->m_inverse_bind_transform;
    _joint->m_animated_transform = currentTransform;
}

std::vector<keyframe> tna_model::get_prev_and_next_frame()
{
    std::vector<keyframe> allFrames = m_key_frames;
    keyframe previousFrame = allFrames[0];
    keyframe nextFrame = allFrames[0];
    for (int i = 1; i < allFrames.size(); i++)
    {
        nextFrame = allFrames[i];
        if (nextFrame.m_time_stamp > m_animation_time)
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

float tna_model::calculate_progression(keyframe previousFrame, keyframe nextFrame)
{
    float totalTime = nextFrame.m_time_stamp - previousFrame.m_time_stamp;
    float currentTime = m_animation_time - previousFrame.m_time_stamp;
    return currentTime / totalTime;
}

std::map<std::string, mat4f> tna_model::interpolate_poses(keyframe previousFrame, keyframe nextFrame, float progression)
{
    std::map<std::string, joint_transform>::iterator it;
    std::map<std::string, mat4f> currentPose;

    for (it = previousFrame.m_poses->begin(); it != previousFrame.m_poses->end(); it++)
    {
        joint_transform previousTransform = (*previousFrame.m_poses)[it->first];
        joint_transform nextTransform = (*nextFrame.m_poses)[it->first];
        joint_transform currentTransform = interpolate(previousTransform, nextTransform, progression);
        currentPose[it->first] = currentTransform.get_local_transform();
    }

    return currentPose;
}

joint_transform tna_model::interpolate(joint_transform frameA, joint_transform frameB, float progression)
{
    return joint_transform(vec3f(frameA.get_pos().interpolate(frameB.get_pos(), progression)), frameA.get_rot().interpolate(frameB.get_rot(), progression));
}