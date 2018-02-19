#ifndef  EARCLIPPING_POLYGON_H_

#include "define.h"
#include "graph_structs.h"
#include <unordered_set>
#include <unordered_map>

using namespace std;

class EarclippingPolygon
{
	public:
		EarclippingPolygon();

		void initFromUnprocessedVertices(vector<Vertex> verticesIn);

		vector<Vertex> vertices;
		vector<Vertex> convexVertices;
		vector<Vertex> reflectVertices;
		vector<int> ears;


		void addNeighborsForVertices();
		void addVertex(glm::vec2 point, int newId);
		int getNewVertexId();

		void init();
		void print();

		void triangulate();
		void calculateInsideAngle(Vertex& v);
		unordered_map<int, int> debugMap;

};


#endif // ! EARCLIPPING_POLYGON_H_
