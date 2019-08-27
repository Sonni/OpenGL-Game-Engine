#ifndef INC_3D_ENGINE_OBJ_LOADER_HPP
#define INC_3D_ENGINE_OBJ_LOADER_HPP

#include <fstream>
#include "indexed_model.hpp"
#include "mesh.hpp"
#include "../../core_engine/util/util.hpp"

class obj_index
{
public:
    int m_vertex_index;
    int m_tex_coord_index;
    int m_normal_index;

    int hashCode()
    {
        int BASE = 17;
        int MULTIPLIER = 31;

        int result = BASE;

        result = MULTIPLIER * result + m_vertex_index;
        result = MULTIPLIER * result + m_tex_coord_index;
        result = MULTIPLIER * result + m_normal_index;

        return result;
    }
};

class obj_loader {
public:
    std::vector<vec3f> m_positions;
    std::vector<vec2f> m_texCoords;
    std::vector<vec3f> m_normals;
    std::vector<obj_index> m_indices;

    bool m_has_tex_coord = false;
    bool m_has_normals = false;

    std::string mesh_name;

    obj_loader(const std::string& file_name)
    {

        std::string path = "../res/models/" + file_name;
        std::ifstream file;
        file.open(path);


        if(file.is_open())
        {
            std::string line;
            mesh_name = path;
            while(file.good())
            {
                getline(file, line);

                std::vector<std::string> tokens = util::split_string(line, ' ');
                //tokens = Util.RemoveEmptyStrings(tokens);

                if(tokens.empty() || tokens[0] == "#")
                    continue;
                else if(tokens[0] == "v")
                {
                    m_positions.push_back(vec3f(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
                }
                else if(tokens[0] == "vt")
                {
                    m_texCoords.push_back(vec2f(std::stof(tokens[1]), 1.0f - std::stof(tokens[2])));
                }
                else if(tokens[0] == "vn")
                {
                    m_normals.push_back(vec3f(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
                }
                else if(tokens[0] == "f")
                {
                    for(int i = 0; i < tokens.size() - 3; i++)
                    {
                        m_indices.push_back(parse_obj_index(tokens[1]));
                        m_indices.push_back(parse_obj_index(tokens[2 + i]));
                        m_indices.push_back(parse_obj_index(tokens[3 + i]));
                    }
                }
            }

            file.close();
        }
        else
        {
            std::cerr << "Unable to load mesh: " << path << std::endl;
        }
    }

    mesh* gen_mesh()
    {
        indexed_model result;
        indexed_model normalModel;
        std::map<int, int> resultIndexMap;
        std::map<int, int> normalIndexMap;
        std::map<int, int> indexMap;

        for(int i = 0; i < m_indices.size(); i++)
        {
            obj_index currentIndex = m_indices[i];

            vec3f currentPosition = m_positions[currentIndex.m_vertex_index];
            vec2f currentTexCoord;
            vec3f currentNormal;

            if(m_has_tex_coord)
                currentTexCoord = m_texCoords[currentIndex.m_tex_coord_index];
            else
                currentTexCoord = vec2f();

            if(m_has_normals)
                currentNormal = m_normals[currentIndex.m_normal_index];
            else
                currentNormal = vec3f();

            auto iter = resultIndexMap.find(currentIndex.hashCode());
            int modelVertexIndex;
            if(iter == resultIndexMap.end())
            {
                modelVertexIndex = result.get_positions().size();
                resultIndexMap[currentIndex.hashCode()] = modelVertexIndex;

                result.add_vertex(currentPosition);
                result.add_tex_coord(currentTexCoord);
                if(m_has_normals)
                    result.add_normal(currentNormal);
            } else {
                modelVertexIndex = iter->second;
            }

            auto iter2 = normalIndexMap.find(currentIndex.m_vertex_index);
            int normalModelIndex = normalIndexMap[currentIndex.m_vertex_index];

            if(iter2 == normalIndexMap.end())
            {
                normalModelIndex = normalModel.get_positions().size();
                normalIndexMap[currentIndex.m_vertex_index] = normalModelIndex;

                normalModel.add_vertex(currentPosition);
                normalModel.add_tex_coord(currentTexCoord);
                normalModel.add_normal(currentNormal);
                //normalModel.add_tangent(vec3f());
            }

            result.add_indice(modelVertexIndex);
            normalModel.add_indice(normalModelIndex);
            indexMap[modelVertexIndex] = normalModelIndex;
        }

        return new mesh(mesh_name, result.finalize());
    }

    obj_index parse_obj_index(std::string token)
    {
        std::vector<std::string> values = util::split_string(token, '/');

        obj_index result;
        result.m_vertex_index = std::stoi(values[0]) - 1;

        if(values.size() > 1)
        {
            if(values[1] != "")
            {
                m_has_tex_coord = true;
                result.m_tex_coord_index = std::stoi(values[1]) - 1;
            }

            if(values.size() > 2)
            {
                m_has_normals = true;
                result.m_normal_index = std::stoi(values[2]) - 1;
            }
        }

        return result;
    }
private:

};


#endif //INC_3D_ENGINE_OBJ_LOADER_HPP
