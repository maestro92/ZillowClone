#ifndef  EARCLIPPING_POLYGON_H_

#include "define.h"
#include "graph_structs.h"
#include "utility_math.h"
#include <unordered_set>
#include <unordered_map>

using namespace std;

class EarclippingPolygon
{
	public:
		EarclippingPolygon();

		void initFromUnprocessedVertices(vector<Vertex> verticesIn);

		vector<Vertex> vertices;
		vector<Vertex> backupVertices;

		vector<int> removed;
		vector<int> convexVertices;
		vector<int> reflexVertices;
		vector<int> ears;

		void findConvexReflexAndEars();

		bool isRemoved(int id);
		void saveOriginalVertices();
		void addTriangleFromEarclipping(Vertex v0, Vertex v1, Vertex v2);
		void printConvexVertices();
		void printReflexVertices();
		void printEars();


		vector<int> getTriangleFormedByAdjacentVerticies(int id);

		void addNeighborsForVertices();
		void addVertex(glm::vec2 point, int newId);
		int getNewVertexId();
		void findEars();
		bool isEar(int i);

		bool checkIfOtherVerticesAreInTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) const;
		void init();
		void print();

		void triangulate();
		void calculateInteriorAngle(Vertex& v);
		unordered_map<int, int> debugMap;

		vector<vector<Vertex>> triangles;


};


#endif // ! EARCLIPPING_POLYGON_H_
