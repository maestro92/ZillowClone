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

	void print()
	{
		cout << "	" << p0.x << " " << p0.y << ",		 " << p1.x << " " << p1.y << endl;
	}
};


struct Vertex
{
	glm::vec2 coord;
	int id;
	vector<int> neighbors;

	void addNeighbor(int idIn)
	{
		neighbors.push_back(idIn);
	}

	void print()
	{
		cout << "myId is  " << id << " " << coord.x << " " << coord.y << endl;
		cout << " my neighbors are " << endl;
		for (int i = 0; i < neighbors.size(); i++)
		{
			cout << neighbors[i] << " ";
		}
	}

};

struct Edge
{
	int id0;
	int id1;

	void print()
	{
		cout << id0 << " " << id1 << endl;
	}
};



#endif // ! GRAPH_STRUCTS_H_
