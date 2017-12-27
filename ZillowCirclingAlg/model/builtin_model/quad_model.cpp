#include "quad_model.h"

/*
QuadModel::QuadModel()
{}

QuadModel::~QuadModel()
{}

QuadModel::QuadModel(int w, int h)
{
	init(0.0, w,
		0.0, h,
		0, 0, 1);
}

QuadModel::QuadModel(int w, int h, float uv_x, float uv_y, float uv_w)
{
	init(0.0, w,
		0.0, h,
		uv_x, uv_y, uv_w);
}

QuadModel::QuadModel(float l, float r,
	float b, float t)
{
	init(l, r,
		b, t,
		0, 0, 1);
}


QuadModel::QuadModel(float l, float r,
	float b, float t,
	float uv_x, float uv_y, float uv_w)
{
	init(l, r,
		b, t,
		uv_x, uv_y, uv_w);
}

void QuadModel::init(float l, float r,
	float b, float t,
	float uv_x, float uv_y, float uv_w)
{
	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	VertexData tmp;

	float uv_x0 = uv_x;  float uv_x1 = uv_x + 1;
	float uv_y0 = uv_y;  float uv_y1 = uv_y + 1;


	/// 0. bot left
	tmp.m_position = glm::vec3(l, b, 0.0);
	tmp.m_UV = glm::vec2(uv_x0 * uv_w, uv_y0 * uv_w);
	vertices.push_back(tmp);
	/// 1. bot right
	tmp.m_position = glm::vec3(r, b, 0.0);
	tmp.m_UV = glm::vec2(uv_x1 * uv_w, uv_y0 * uv_w);
	vertices.push_back(tmp);

	/// 2. top right
	tmp.m_position = glm::vec3(r, t, 0.0);
	tmp.m_UV = glm::vec2(uv_x1 * uv_w, uv_y1 * uv_w);
	vertices.push_back(tmp);
	/// 3. top left
	tmp.m_position = glm::vec3(l, t, 0.0);
	tmp.m_UV = glm::vec2(uv_x0 * uv_w, uv_y1 * uv_w);
	vertices.push_back(tmp);

	initIndices(indices);

	Mesh m(vertices, indices);
	m_meshes.push_back(m);
}



void QuadModel::initIndices(vector<unsigned int>& indices)
{
	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);
}
*/


#include "quad_model.h"


QuadModel::QuadModel()
{}

QuadModel::~QuadModel()
{}



QuadModel::QuadModel(int i)
{
    init();
}


QuadModel::QuadModel(int w, int h) : QuadModel(w, h, 0, 0, 1)
{ }


QuadModel::QuadModel(int w, int h, float uv_x, float uv_y, float uv_w)
{
    /// assigning a quad for rendering textures
    std::vector<unsigned int> indices;
    std::vector<EG_VertexData> vertices;
    EG_VertexData tmp;

    float uv_x0 = uv_x;  float uv_x1 = uv_x + 1;
    float uv_y0 = uv_y;  float uv_y1 = uv_y + 1;


    /// 0. bot left
    tmp.m_position = glm::vec3(0.0, 0.0, 0.0);
    tmp.m_UV = glm::vec2(uv_x0 * uv_w, uv_y0 * uv_w);
    vertices.push_back(tmp);
    /// 1. bot right
    tmp.m_position = glm::vec3(w, 0.0, 0.0);
    tmp.m_UV = glm::vec2(uv_x1 * uv_w, uv_y0 * uv_w);
    vertices.push_back(tmp);

    /// 2. top right
    tmp.m_position = glm::vec3(w, h, 0.0);
    tmp.m_UV = glm::vec2(uv_x1 * uv_w, uv_y1 * uv_w);
    vertices.push_back(tmp);
    /// 3. top left
    tmp.m_position = glm::vec3(0.0, h, 0.0);
    tmp.m_UV = glm::vec2(uv_x0 * uv_w, uv_y1 * uv_w);
    vertices.push_back(tmp);

    initIndices(indices);

    EG_Mesh m(vertices, indices);
    m_meshes.push_back(m);
}



void QuadModel::initIndices(vector<unsigned int>& indices)
{
    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    indices.push_back(2);
    indices.push_back(3);
    indices.push_back(0);
}

void QuadModel::init()
{
    init(1, 1, 0, 0, 1);
}



void QuadModel::init(int w, int h)
{
    init(w, h, 0, 0, 1);
}




void QuadModel::init(int w, int h, float uv_x, float uv_y, float uv_w)
{
    /// assigning a quad for rendering textures
    std::vector<unsigned int> indices;
    std::vector<EG_VertexData> vertices;
    EG_VertexData tmp;

    float uv_x0 = uv_x;  float uv_x1 = uv_x + 1;
    float uv_y0 = uv_y;  float uv_y1 = uv_y + 1;


    /// 0. bot left
    tmp.m_position = glm::vec3(0.0, 0.0, 0.0);
    tmp.m_UV = glm::vec2(uv_x0 * uv_w, uv_y0 * uv_w);
    vertices.push_back(tmp);
    /// 1. bot right
    tmp.m_position = glm::vec3(w, 0.0, 0.0);
    tmp.m_UV = glm::vec2(uv_x1 * uv_w, uv_y0 * uv_w);
    vertices.push_back(tmp);

    /// 2. top right
    tmp.m_position = glm::vec3(w, h, 0.0);
    tmp.m_UV = glm::vec2(uv_x1 * uv_w, uv_y1 * uv_w);
    vertices.push_back(tmp);
    /// 3. top left
    tmp.m_position = glm::vec3(0.0, h, 0.0);
    tmp.m_UV = glm::vec2(uv_x0 * uv_w, uv_y1 * uv_w);
    vertices.push_back(tmp);

    initIndices(indices);

    EG_Mesh m(vertices, indices);
    m_meshes.push_back(m);
}


void QuadModel::init(int w, int h, glm::vec3 c)
{
    init(w, h, c, c, c, c);
}


void QuadModel::init(int w, int h, glm::vec3 c1, glm::vec3 c2, glm::vec3 c3, glm::vec3 c4)
{
    /// assigning a quad for rendering textures


    std::vector<unsigned int> indices;
    std::vector<EG_VertexData> vertices;
    EG_VertexData tmp;

    //1.
    tmp.m_position = glm::vec3(0.0, h, 0.0);
    tmp.m_color = c1;
    vertices.push_back(tmp);
    //2.
    tmp.m_position = glm::vec3(0.0, 0.0, 0.0);
    tmp.m_color = c2;
    vertices.push_back(tmp);

    //3.
    tmp.m_position = glm::vec3(w, 0.0, 0.0);
    tmp.m_color = c3;
    vertices.push_back(tmp);
    //4.
    tmp.m_position = glm::vec3(w, h, 0.0);
    tmp.m_color = c4;
    vertices.push_back(tmp);

    initIndices(indices);

    EG_Mesh m(vertices, indices);
    m_meshes.push_back(m);
}



void QuadModel::initCentered()
{
    init(-0.5, 0.5, -0.5, 0.5,
         0.0, 0.0, 1.0);
}


void QuadModel::init(float l, float r,
                           float b, float t,
                           float cx, float cy, float cw)
{
/// assigning a quad for rendering textures
    m_modelGeometry = GL_TRIANGLES;

    std::vector<unsigned int> indices;
    std::vector<EG_VertexData> vertices;
    EG_VertexData tmp;

    float cx0 = cx;  float cx1 = cx+1;
    float cy0 = cy;  float cy1 = cy+1;

    ///1. left-top
    tmp.m_position = glm::vec3(l, t, 0.0);
    tmp.m_UV = glm::vec2(cx0 * cw, cy0 * cw);
    vertices.push_back(tmp);

    ///2. left-bottom
    tmp.m_position = glm::vec3(l, b, 0.0);
    tmp.m_UV = glm::vec2(cx0 * cw, cy1 * cw);
    vertices.push_back(tmp);

    ///3. right-bottom
    tmp.m_position = glm::vec3(r, b, 0.0);
    tmp.m_UV = glm::vec2(cx1 * cw, cy1 * cw);
    vertices.push_back(tmp);

    ///4. right-top
    tmp.m_position = glm::vec3(r, t, 0.0);
    tmp.m_UV = glm::vec2(cx1 * cw, cy0 * cw);
    vertices.push_back(tmp);

    initIndices(indices);

    EG_Mesh m(vertices, indices);
    m_meshes.push_back(m);
}



