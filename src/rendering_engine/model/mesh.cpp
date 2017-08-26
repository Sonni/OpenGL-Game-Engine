#include "mesh.h"
#include "../../../../Game-Engine/src/profiling.h"

#include <cassert>
#include "tnaLoader.h"


std::map<std::string, mesh_data*> mesh::s_resourceMap;

mesh_data::mesh_data(const indexed_model& model) :
	m_drawCount((int) model.get_indices().size())
{
	/*if(!model.is_valid())
	{
		std::cout << "Model is not valid" << std::endl;
		assert(0 != 0);
	}*/

    
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(NUM_BUFFERS, vab);
	glBindBuffer(GL_ARRAY_BUFFER, vab[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.get_positions().size() * sizeof(model.get_positions()[0]), &model.get_positions()[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vab[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.get_tex_coords().size() * sizeof(model.get_tex_coords()[0]), &model.get_tex_coords()[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	
	glBindBuffer(GL_ARRAY_BUFFER, vab[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, model.get_normals().size() * sizeof(model.get_normals()[0]), &model.get_normals()[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	


    if(model.has_animation())
    {


        glBindBuffer(GL_ARRAY_BUFFER, vab[BONEIDS_VB]);
        glBufferData(GL_ARRAY_BUFFER, model.get_bone_ids().size() * sizeof(model.get_bone_ids()[0]), &model.get_bone_ids()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 3, GL_INT, GL_FALSE, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vab[WEIGHTS_VB]);
        glBufferData(GL_ARRAY_BUFFER, model.get_weights().size() * sizeof(model.get_weights()[0]), &model.get_weights()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vab[INDEX_VB]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.get_indices().size() * sizeof(model.get_indices()[0]),
                     &model.get_indices()[0], GL_STATIC_DRAW);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, vab[TANGENT_VB]);
        glBufferData(GL_ARRAY_BUFFER, model.get_tangents().size() * sizeof(model.get_tangents()[0]), &model.get_tangents()[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vab[INDEX_VB]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, model.get_indices().size() * sizeof(model.get_indices()[0]),
                     &model.get_indices()[0], GL_STATIC_DRAW);
    }
}

mesh_data::~mesh_data()
{	
	glDeleteBuffers(NUM_BUFFERS, vab);
	glDeleteVertexArrays(1, &vao);
}

void mesh_data::draw() const
{
	glBindVertexArray(vao);

    glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
}


mesh::mesh(const std::string& meshName, const indexed_model& model) :
        m_fileName(meshName)
{
    std::map<std::string, mesh_data*>::const_iterator it = s_resourceMap.find(meshName);
    if(it != s_resourceMap.end())
    {
        std::cout << "Error adding mesh " << meshName << ": Mesh already exists by the same name!" << std::endl;
        assert(0 != 0);
    }
    else
    {
        m_meshData = new mesh_data(model);
        s_resourceMap.insert(std::pair<std::string, mesh_data*>(meshName, m_meshData));
    }
}


mesh::mesh(const std::string& fileName, bool flipUVS)
{
    m_fileName = fileName;
    m_meshData = 0;

    std::map<std::string, mesh_data*>::const_iterator it = s_resourceMap.find(fileName);
    if(it != s_resourceMap.end())
    {
        m_meshData = it->second;
    }
    else
    {
        TNAModel tnaModel(fileName);
        indexed_model model = tnaModel.to_indexed_model();
        has_animation = tnaModel.has_animation;

        if (flipUVS)
        {
            for (int i = 0; i < model.get_tex_coords().size(); i++)
				model.get_tex_coords()[i].set_y(1 - model.get_tex_coords()[i].get_y()); //Flip UV
        }

        //std::vector<Vector3f> tangents = calc_tangents(model);


        if (has_animation)
            m_meshData = new mesh_data(model);
        else
            m_meshData = new mesh_data(model.finalize());
        s_resourceMap.insert(std::pair<std::string, mesh_data*>(fileName, m_meshData));
    }
}



mesh::mesh(const mesh& Mesh) :
	m_fileName(Mesh.m_fileName),
	m_meshData(Mesh.m_meshData)
{
}

mesh::~mesh()
{
	if(m_meshData)
	{
		if(m_fileName.length() > 0)
		{
			s_resourceMap.erase(m_fileName);
		}
			
		delete m_meshData;
	}
}

void mesh::draw() const
{
    m_meshData->draw();
}
