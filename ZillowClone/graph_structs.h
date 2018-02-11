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
		cout << "		" << p0.x << " " << p0.y << ",		 " << p1.x << " " << p1.y << endl;
	}
};


struct Vertex
{
	glm::vec2 pos;
	int id;
	vector<int> neighbors;

	void addNeighbor(int idIn)
	{
		neighbors.push_back(idIn);
	}

	int getFirstNeighborThatIsNot(int idIn)
	{
		for (int i = 0; i < neighbors.size(); i++)
		{
			if (neighbors[i].id != idIn)
			{
				return neighbors[i].id;
			}
		}
		return -1;
	}

	void print()
	{
		cout << "myId is  " << id << ", Coords: " << pos.x << " " << pos.y << endl;
		cout << " my neighbors are " << endl;
		for (int i = 0; i < neighbors.size(); i++)
		{
			cout << neighbors[i] << " ";
		}
		cout << endl;
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
