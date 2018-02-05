#ifndef GRAPH_STRUCTS_H_
#define GRAPH_STRUCTS_H_


#include <stdlib.h>

#include "define.h"
using namespace std;

#include <vector>


struct Line
{
	glm::vec2 p0;
	glm::vec2 p1;

	Line()
	{}

	Line(glm::vec2 p0In, glm::vec2 p1In)
	{
		p0 = p0In;
		p1 = p1In;
	}
};


struct Vertex
{
	glm::vec2 coord;
	int id;
	vector<int> neighbors;
};

struct Edge
{
	int id0;
	int id1;
};



#endif // ! GRAPH_STRUCTS_H_
