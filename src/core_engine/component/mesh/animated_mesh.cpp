#include "animated_mesh.h"

void set_attribute(GLuint attrib_num, int coord_size, std::vector<float> data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attrib_num, coord_size, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void set_attribute(GLuint attrib_num, int coord_size, std::vector<vec3f> data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attrib_num, coord_size, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void set_attribute(GLuint attrib_num, int coord_size, std::vector<vec2f> data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    glVertexAttribPointer(attrib_num, coord_size, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void set_attribute(GLuint attrib_num, int coord_size, std::vector<int> data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    glVertexAttribIPointer(attrib_num, coord_size, GL_INT, GL_FALSE, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void set_attribute(GLuint attrib_num, int coord_size, std::vector<vec3i> data)
{
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), &data[0], GL_STATIC_DRAW);
    glVertexAttribIPointer(attrib_num, coord_size, GL_INT, GL_FALSE, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}




std::vector<float> to_array(std::vector<vec3f> v)
{
    std::vector<float> r;
    for (int i = 0; i < v.size(); i++)
    {
        r.push_back(v[i].get_x());
        r.push_back(v[i].get_y());
        r.push_back(v[i].get_z());
    }
    return r;
}

std::vector<unsigned int> to_array(std::vector<vec3i> v)
{
    std::vector<unsigned int> r;
    for (int i = 0; i < v.size(); i++)
    {
        r.push_back((unsigned int) v[i].get_x());
        r.push_back((unsigned int) v[i].get_y());
        r.push_back((unsigned int) v[i].get_z());
    }
    return r;
}

std::vector<float> to_array(std::vector<vec2f> v)
{
    std::vector<float> r;
    for (int i = 0; i < v.size(); i++)
    {
        r.push_back(v[i].get_x());
        r.push_back(v[i].get_y());
    }
    return r;
}

animation_component::animation_component(const std::string& file_name, mat4f* shadowMap, texture* depth_map, physics_obj* phy_obj, const std::string& tex_file_name, const std::string& normal_map) :
        _mesh(file_name),
        tna_model(file_name),
        phy_obj(phy_obj)
{
    this->shadow_mvp = shadowMap;
    this->depth_map = depth_map;
    tex = new texture("diffuse.png");
/*

    vertex_count = tna_model.m_vertices.size();


    std::vector<float> vert = to_array(tna_model.m_vertices);


    //std::vector<unsigned int> ind = to_array(tna_model.small_ind);
    std::vector<vec3i> ind = tna_model.m_ind;

    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);

    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(ind[0]), &ind[0], GL_STATIC_DRAW);




    set_attribute(0, 3, tna_model.m_vertices);
    set_attribute(1, 2, tna_model.m_uvs);
    set_attribute(2, 3, tna_model.m_normals);
    set_attribute(3, 3, tna_model.joint_ids);
    set_attribute(4, 3, tna_model.m_weights);

    glBindVertexArray(0);*/
}

animation_component::~animation_component()
{
    delete tex;
}


void animation_component::init()
{
    glBindAttribLocation(get_shader()->get_program_id(), 0, "in_position");
    glBindAttribLocation(get_shader()->get_program_id(), 1, "in_textureCoords");
    glBindAttribLocation(get_shader()->get_program_id(), 2, "in_normal");
    glBindAttribLocation(get_shader()->get_program_id(), 3, "in_jointIndices");
    glBindAttribLocation(get_shader()->get_program_id(), 4, "in_weights");



    get_shader()->add_uniform("diffuseMap");
    get_shader()->add_uniform("projectionViewMatrix");

   for (int i = 0; i < 50; i++)
    {
        std::ostringstream spotLightNameBuilder;
        spotLightNameBuilder << "jointTransforms[" << i << "]";
        std::string n = spotLightNameBuilder.str();

        get_shader()->add_uniform(n);
    }


}

void animation_component::set_all_uni(camera& cam)
{

    get_shader()->set_uniform_mat4f("projectionViewMatrix", cam.get_view_projection() * get_transform()->get_transformation());


    tex->bind(0);
    get_shader()->set_uniform_1i("diffuseMap", 0);


}

float counter = 0;
float animationCounter = 0;
float leftOver = 0;

void animation_component::add_child(joint* r) const
{

    mat4f ani = r->animatedTransform;

    //std::cout << r->name << " : " << r->id << " : " << ani << std::endl;


    get_shader()->set_uniform_mat4f("jointTransforms[" + std::to_string(r->id) + "]", ani);

    for (joint* j : r->m_children)
    {
        add_child(j);
    }

}

void animation_component::update(float delta, const camera &cam)
{

    tna_model.update();


}

void animation_component::render() const
{


    add_child(tna_model.m_root);

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glDisable(GL_CULL_FACE);

/*
    glBindVertexArray(vao_id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, 0);


    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    */

    _mesh.draw();


}