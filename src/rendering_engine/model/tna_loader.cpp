#include "tna_loader.h"
#include "../../core_engine/util/util.h"
#include <fstream>


tna_model::tna_model(const std::string& fileName)
{
    std::setprecision(9);

    m_fileName = fileName;
    std::string path = "../res/models/" + fileName + ".tna";
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
                        Loader::load_joint_ids(line, joint_ids);
                    if(lineCStr[1] == 'o')
                    {
                        has_animation = true;
                        Loader::load_joints(line, m_root);

                    }
                    break;

                case 'k':
                    Loader::load_keyframe(line, keyframes);
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
    result.set_is_animated(has_animation);


    if (has_animation)
    {
        for (int i = 0; i < vertices.size(); i++)
        {
            result.add_vertex(vec3f(vertices[i]));
            result.add_normal(vec3f(normals[i]));
            result.add_tex_coord(vec2f(uvs[i]));

            result.add_bone_id(vec3i(joint_ids[i * 3], joint_ids[i * 3 + 1], joint_ids[i * 3 + 2]));
            result.add_weight(vec3f(m_weights[i * 3], m_weights[i * 3 + 1], m_weights[i * 3 + 2]));
        }

        for (int i = 0; i < reduced_indices.size(); i++)
        {
            result.add_indice(reduced_indices[i].get_x());
            result.add_indice(reduced_indices[i].get_y());
            result.add_indice(reduced_indices[i].get_z());
        }
    }
    else
    {
        for (int i = 0; i < indices.size(); i++)
        {
            result.add_vertex(vec3f(vertices[indices[i].get_x()]));
            result.add_normal(vec3f(normals[indices[i].get_y()]));
            result.add_tex_coord(vec2f(uvs[indices[i].get_z()]));
            result.add_indice(i);
        }
    }


    if (!has_animation) result.calc_tangents();

    return result;
}

void tna_model::load_vertices(const std::string &line)
{
    std::string vert_line = line;
    vert_line.erase(0, 3);
    
    std::vector<std::string> vertArray = util::split_string(vert_line, ' ');
    for(int i = 0; i < vertArray.size(); i += 3)
        vertices.push_back(vec3f(std::stof(vertArray[i]), std::stof(vertArray[i + 1]), std::stof(vertArray[i + 2])));
}

void tna_model::load_uvs(const std::string &line)
{
    std::string uv_line = line;
    uv_line.erase(0, 4);
    
    std::vector<std::string> uvArray = util::split_string(uv_line, ' ');
    for(int i = 0; i < uvArray.size(); i += 2)
        uvs.push_back(vec2f(std::stof(uvArray[i]), std::stof(uvArray[i + 1])));
}

void tna_model::load_normals(const std::string &line)
{
    std::string normale_line = line;
    normale_line.erase(0, 4);
    
    std::vector<std::string> normalArray = util::split_string(normale_line, ' ');
    for(int i = 0; i < normalArray.size(); i += 3)
        normals.push_back(vec3f(std::stof(normalArray[i]), std::stof(normalArray[i + 1]), std::stof(normalArray[i + 2])));
}

void tna_model::load_faces(const std::string &line)
{
    std::string faces = line;
    faces.erase(0, 3);

    std::vector<std::string> faceArray = util::split_string(faces, ' ');

    for(int i = 0; i < faceArray.size(); i += 3)
    {
        indices.push_back(vec3i(stoi(faceArray[i]), stoi(faceArray[i+1]), stoi(faceArray[i+2])));
    }
}

void tna_model::load_reduced_faces(const std::string &line)
{
    std::string faces = line;
    faces.erase(0, 4);

    std::vector<std::string> faceArray = util::split_string(faces, ' ');

    for(int i = 0; i < faceArray.size(); i += 3)
    {

        reduced_indices.push_back(vec3i(stoi(faceArray[i]), stoi(faceArray[i+1]), stoi(faceArray[i+2])));

    }
}

void tna_model::load_animation_len(const std::string &line)
{
    animation_len = stof(util::split_string(line, ' ')[1]);
}