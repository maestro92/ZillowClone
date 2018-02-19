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
	float angle;
	
	vector<int> neighbors;

	Vertex()
	{
		id = -1;
	}

	void addNeighbor(int idIn)
	{
//		cout << "Vertex " << id << " is adding " << idIn << endl;
		neighbors.push_back(idIn);
	}

	void removeNeighbor(int idIn)
	{
		for (int i = 0; i < neighbors.size(); i++)
		{
			if (neighbors[i] == idIn)
			{
				neighbors.erase(neighbors.begin() + i);
			}
		}
	}

	void resetNeighbors()
	{
		neighbors.clear();
	}

	bool hasNeighbors(int idIn)
	{
		for (int i = 0; i < neighbors.size(); i++)
		{
			if (neighbors[i] == idIn)
			{
				return true;
			}
		}
		return false;
	}

	int getFirstNeighborThatIsNot(int idIn)
	{
		for (int i = 0; i < neighbors.size(); i++)
		{
			if (neighbors[i] != idIn)
			{
				return neighbors[i];
			}
		}
		return -1;
	}

	bool isValid()
	{
		return id != -1;
	}

	void print()
	{
		cout << "myId is " << id << ", Coords: " << pos.x << " " << pos.y << endl;
		cout << "	I have " << neighbors.size() << endl;
		for (int i = 0; i < neighbors.size(); i++)
		{
			cout << neighbors[i] << " ";
		}
		cout << endl << endl;
	}

	bool operator==(const Vertex &other) const 
	{
		return id == other.id;
	}

	bool operator!=(const Vertex &other) const
	{
		return id != other.id;
	}

};

struct Edge
{
	int id0;
	int id1;

	Edge()
	{}


	Edge(int id0In, int id1In)
	{
		id0 = id0In;
		id1 = id1In;
	}

	void print()
	{
		cout << id0 << " " << id1 << endl;
	}

	bool operator==(const Edge &other) const
	{
		return id0 == other.id0 && id1 == other.id1 || id0 == other.id1 && id1 == other.id0;
	}
};



#endif // ! GRAPH_STRUCTS_H_
