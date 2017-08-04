#include "tnaLoader.h"
#include <fstream>

TNAModel::TNAModel(const std::string& fileName)
{
    m_fileName = fileName;
    std::string path = "../res/models/" + fileName + ".tna";
    std::ifstream file;
    file.open(path);
    
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
                        LoadUvs(line);
                    else if(lineCStr[1] == 'n')
                        LoadNormals(line);
                    else if(lineCStr[1] == ':' || lineCStr[1] == '\t')
                        LoadVertices(line);
                    break;
                case 'f':
                    LoadFaces(line);
                    break;
                case 'b':
                    has_animation = true;
                    if(lineCStr[1] == 'o')
                    {
                        Loader::LoadBones(line, m_bones, m_idPointer);
                        m_idPointer++;
                    }
                    if(lineCStr[1] == 'i')
                        Loader::LoadBindShape(line, m_bindShapeMatrix);
                    break;
                case 'w':
                    Loader::LoadWeights(line, m_weights);
                    break;
                case 'i':
                    if(lineCStr[1] == 'd')
                        Loader::LoadBoneArray(line, m_boneIds);
                    else if(lineCStr[1] == 'n')
                        Loader::LoadInvBindMatrix(line, m_bones, m_boneIds);
                    break;
                case 'j':
                    if(lineCStr[1] == 'c')
                        Loader::LoadJCount(line, m_jCount);
                    else if(lineCStr[1] == ':')
                        Loader::LoadJs(line, m_js);
                    break;
                case 't':
                    m_animation.push_back(AnimationData());
                    Loader::LoadTime(line, m_animation[m_animation.size() - 1].m_time, m_animation[m_animation.size() - 1].m_bone);
                    break;
                case 'a':
                    Loader::LoadAnimation(line, m_animation[m_animation.size() - 1].m_transform);
                    break;
                default: break;
            };
        }
    }
    else
    {
        std::cerr << "Unable to load mesh: " << path << std::endl;
    }

    if (has_animation) {
        CalcBonesWorldMatrix();
        SetupAnimation(20);
    }
}

indexed_model TNAModel::to_indexed_model()
{
    indexed_model result;
    result.set_is_animated(has_animation);
    std::vector<Skinning> tmpSkinning;
    if (has_animation) {
        int pointer = 0;
        for (int i = 0; i < m_jCount.size(); i++) {
            int size = m_jCount[i];
            Skinning tmpSkin;
            tmpSkin.m_id = i;
            for (int j = 0; j < size; j++) {
                tmpSkin.m_boneId.push_back(m_bones[m_js[pointer]].m_uniqueId);
                pointer++; //This might be the wrong bone id
                tmpSkin.m_weights.push_back(m_weights[m_js[pointer]]);
                pointer++;
            }

            tmpSkinning.push_back(tmpSkin);
        }
    }

    
    for(int i = 0; i < m_TNAIndices.size(); i++)
    {
        result.add_vertex(vec3f(m_vertices[m_TNAIndices[i].vertexIndex]));
        result.add_normal(vec3f(m_normals[m_TNAIndices[i].normalIndex]));
        result.add_tex_coord(vec2f(m_uvs[m_TNAIndices[i].uvIndex]));
        result.add_indice(i);
        if (has_animation) m_skinning.push_back(tmpSkinning[m_TNAIndices[i].vertexIndex]);
    }

    //Setting up ids and weights
    if (has_animation) {
        std::vector<vec4i> boneIds;
        std::vector<vec4f> weights;

        for (int i = 0; i < m_skinning.size(); i++) // for every vertex
        {
            float tmpW[4];
            float tmpJ[4];
            for (int j = 0; j < 4; j++) {
                tmpW[j] = 0;
                tmpJ[j] = 0;
            }

            for (int j = 0; j < m_skinning[i].m_weights.size(); j++) // influence count
            {
                tmpJ[j] = m_skinning[i].m_boneId[j];
                tmpW[j] = m_skinning[i].m_weights[j];
            }
            result.add_bone_id(vec4i(tmpJ[0], tmpJ[1], tmpJ[2], tmpJ[3]));
            result.add_weight(vec4f(tmpW[0], tmpW[1], tmpW[2], tmpW[3]));
        }
    }

    result.calc_tangents();

    return result;
}

void TNAModel::CalcBonesWorldMatrix(int frame)
{
    if(frame != -1)
        for(int i = 0; i < m_bones.size(); i++)
            for(int j = 0; j < m_animation.size(); j++)
                if(m_animation[j].m_bone == m_bones[i].m_id)
                    m_bones[i].m_jointMatrix = m_animation[i].m_transform[frame];
    
    
    for(int i = 0; i < m_bones.size(); i++)
        if(m_bones[i].m_id == m_bones[i].m_parentId) //if parentid and id is the same, then it's the root bone
        {
            m_bones[i].m_worldMatrix = m_bones[i].m_jointMatrix;
            m_bones[i].m_skinningMatrix = m_bones[i].m_invBindMatrix.transpose() * m_bones[i].m_worldMatrix.transpose();

            for(int j = 0; j < m_bones[i].m_children.size(); j++)
                CalcChildWorldMat(m_bones[i].m_id, m_bones[i].m_children[j], m_bones[i].m_worldMatrix);
            
            break; //the root and it's children has been processed, so we break the for loop
        }
}

void TNAModel::CalcChildWorldMat(const std::string& parentId, const std::string& childId, mat4f parentWorldMatrix)
{
    
    for(int i = 0; i < m_bones.size(); i++)
        if(m_bones[i].m_parentId == parentId && m_bones[i].m_id == childId) //if the currents bones id and parents id matches the arguments, then it is the correct child.
        {
            m_bones[i].m_worldMatrix = parentWorldMatrix * m_bones[i].m_jointMatrix;
            m_bones[i].m_skinningMatrix = m_bones[i].m_invBindMatrix.transpose() * m_bones[i].m_worldMatrix.transpose();
            
            for(int j = 0; j < m_bones[i].m_children.size(); j++)
                CalcChildWorldMat(m_bones[i].m_id, m_bones[i].m_children[j], m_bones[i].m_worldMatrix);
            
            break;//the child has been processed we can break the loop
        }
}

void TNAModel::LoadVertices(const std::string& line)
{
    std::string vertices = line;
    vertices.erase(0, 3);
    
    std::vector<std::string> vertArray = Loader::Split(vertices, ' ');
    for(int i = 0; i < vertArray.size(); i += 3)
        m_vertices.push_back(vec3f(std::stof(vertArray[i]), std::stof(vertArray[i + 1]), std::stof(vertArray[i + 2])));
}

void TNAModel::LoadUvs(const std::string& line)
{
    std::string uvs = line;
    uvs.erase(0, 4);
    
    std::vector<std::string> uvArray = Loader::Split(uvs, ' ');
    for(int i = 0; i < uvArray.size(); i += 2)
        m_uvs.push_back(vec2f(std::stof(uvArray[i]), std::stof(uvArray[i + 1])));
}

void TNAModel::LoadNormals(const std::string& line)
{
    std::string normals = line;
    normals.erase(0, 4);
    
    std::vector<std::string> normalArray = Loader::Split(normals, ' ');
    for(int i = 0; i < normalArray.size(); i += 3)
        m_normals.push_back(vec3f(std::stof(normalArray[i]), std::stof(normalArray[i + 1]), std::stof(normalArray[i + 2])));
}

void TNAModel::LoadFaces(const std::string& line)
{
    std::string faces = line;
    faces.erase(0, 3);
    
    std::vector<std::string> faceArray = Loader::Split(faces, ' ');
    
    for(int i = 0; i < faceArray.size(); i += 3)
    {
        TNAIndex result;
        
        result.vertexIndex = stoi(faceArray[i]);
        result.normalIndex = stoi(faceArray[i + 1]);
        result.uvIndex = stoi(faceArray[i + 2]);
        
        this->m_TNAIndices.push_back(result);
    }
}


