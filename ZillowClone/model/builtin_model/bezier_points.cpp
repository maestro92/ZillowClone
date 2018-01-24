#include "bezier_points.h"

BezierPoints::BezierPoints()
{
	m_modelGeometry = GL_LINES;

	std::vector<VertexData> vertices;
	std::vector<unsigned int> indices;
	VertexData v;

	int numSegment = 20;

	glm::vec3 p0 = glm::vec3(-4.0, -4.0, 0.0);
	glm::vec3 p1 = glm::vec3(-2.0, 4.0, 0.0);
	glm::vec3 p2 = glm::vec3(2.0, -4.0, 0.0);
	glm::vec3 p3 = glm::vec3(4.0, 4.0, 0.0);

	float t = 0;
	for (int i = 0; i <= numSegment; i++)
	{
		t = i / (float)numSegment;
		glm::vec3 pixel = calculateBezierPoint(t, p0, p1, p2, p3);
		
		v.m_position = pixel;
		v.m_color = glm::vec3(1.0, 0.0, 0.0);
		vertices.push_back(v);
	}

	for (int i = 0; i < numSegment; i++)
	{
		indices.push_back(i);
		indices.push_back(i+1);
	}


	//   worldAxis = new mesh(&axisVertices, &axisIndices);
	Mesh m(vertices, indices);
	m_meshes.push_back(m);
}

BezierPoints::~BezierPoints()
{
}


// [x,y] = (1–t)^3 * P0 + 3 * (1–t)^2 * t * P1 + 3(1–t) * (t^2) * P2+ t^3 * P3
glm::vec3 BezierPoints::calculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	float u = 1 - t;
	float uu = u*u;
	float uuu = uu * u;

	float tt = t * t;
	float ttt = tt * t;

	glm::vec3 p = uuu * p0;	// first term
	p += 3 * uu * t * p1;	// second term
	p += 3 * u * tt * p2;	// third term
	p += ttt * p3;			// fourth term 

	return p;
}

