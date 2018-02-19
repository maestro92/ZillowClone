#include "earclipping_polygon.h"



EarclippingPolygon::EarclippingPolygon()
{

}

int EarclippingPolygon::getNewVertexId()
{
	return vertices.size();
}



// so that all vertices only have two neighbors, this is possible cuz we already have the order that we traversaed our polygon in
void EarclippingPolygon::initFromUnprocessedVertices(vector<Vertex> verticesTraversalList)
{
	unordered_set<int> visitedIds;
	

	for (int i = 0; i < verticesTraversalList.size(); i++)
	{
		Vertex v = verticesTraversalList[i];

		if (i == 0)
		{
			int newId = getNewVertexId();
			addVertex(v.pos, newId);
			debugMap[newId] = v.id;

		}
		else if (i == verticesTraversalList.size() - 1)
		{
			// dont do anything, cuz we're returning
		}
		else
		{
			int newId = getNewVertexId();
			addVertex(v.pos, newId);

			debugMap[newId] = v.id;
		}


	}
	cout << "vertices.size() " << vertices.size() << endl;

	addNeighborsForVertices();

//	print();
}


void EarclippingPolygon::addNeighborsForVertices()
{
	// now add the neighbors
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex& v = vertices[i];
		
		if (i == 0)
		{
			Vertex prev = vertices[vertices.size() - 1];
			if (!v.hasNeighbors(prev.id))
			{
				v.addNeighbor(prev.id);
			}
	
			Vertex next = vertices[i + 1];
			if (!v.hasNeighbors(next.id))
			{
				v.addNeighbor(next.id);
			}
		}

		else if (i == vertices.size() - 1)
		{
			Vertex prev = vertices[i - 1];
			if (!v.hasNeighbors(prev.id))
			{
				v.addNeighbor(prev.id);
			}

			Vertex next = vertices[0];
			if (!v.hasNeighbors(next.id))
			{
				v.addNeighbor(next.id);
			}
		}
		else
		{
			// dont do anything, cuz we're returning
			Vertex prev = vertices[i - 1];
			Vertex next = vertices[i + 1];

			if (!v.hasNeighbors(prev.id))
			{
				v.addNeighbor(prev.id);
			}
			if (!v.hasNeighbors(next.id))
			{
				v.addNeighbor(next.id);
			}
		}

		calculateInsideAngle(v);
	}
}


void EarclippingPolygon::calculateInsideAngle(Vertex& v)
{
	// orders matter
	if (v.neighbors.size() == 2)
	{
		int prevId = v.neighbors[0];
		int nextId = v.neighbors[1];
		Vertex prev = vertices[prevId];
		Vertex next = vertices[nextId];
	}
	else
	{
		cout << "Something is wrong, num of neighbors is incorrect " << endl;
	}



}


void EarclippingPolygon::print()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		cout << "originally " << debugMap[vertices[i].id] << endl;
		vertices[i].print();
	}
}

void EarclippingPolygon::addVertex(glm::vec2 point, int newId)
{
	Vertex v = Vertex();
	v.id = newId;
	v.pos = point;
	vertices.push_back(v);
}


void EarclippingPolygon::init()
{

}


// Triangulation
void EarclippingPolygon::triangulate()
{

}


