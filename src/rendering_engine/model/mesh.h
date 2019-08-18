#ifndef ___D_rendering_engine__mesh__
#define ___D_rendering_engine__mesh__

#include <map>
#include <GL/glew.h>
#include "indexed_model.h"

class mesh_data
{
public:
	mesh_data(const indexed_model& model);
	virtual ~mesh_data();
	
	void draw() const;
protected:	
private:
	mesh_data(mesh_data& other) {}
	void operator=(mesh_data& other) {}

	enum
	{
		POSITION_VB,
		TEXCOORD_VB,
		NORMAL_VB,
		TANGENT_VB,
        BONEIDS_VB,
        WEIGHTS_VB,
		
		INDEX_VB,
		
		NUM_BUFFERS
	};
	
	GLuint m_vao;
	GLuint m_vab[NUM_BUFFERS];
	int m_drawCount;
    int count;
};

class mesh
{
public:
	mesh(const std::string& meshName, const indexed_model& model, bool is_animated = false);
	mesh(const std::string& fileName, bool flipUVS = false);
	mesh(const mesh& mesh);
	virtual ~mesh();

	void draw() const;

    inline std::string get_file_name() const { return m_fileName; }
    inline bool is_animated() const { return has_animation; }
protected:
private:
	static std::map<std::string, mesh_data*> s_resourceMap;
	mesh_data* m_meshData;
    std::string m_fileName;
	bool has_animation = false;
    
	void operator=(mesh& Mesh) {}
};

#endif