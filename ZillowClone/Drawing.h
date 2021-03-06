#ifndef DRAWING_H_
#define DRAWING_H_

#include "graph_structs.h"
#include "earclipping_polygon.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <functional>
#include "utility.h"
#include "utility_math.h"
#include "utility_json.h"
#include "utility_debug.h"
#include <iostream>
#include <fstream>

class Drawing
{
	public:

		static float EPSILON;
		static float BEST_FIT_ERROR_THRESHOLD;

		int counter;
		struct IntersectionInfo
		{
			glm::vec2 intersectionPoint;

			// the line that he was intersecting with
			glm::vec2 point0;
			glm::vec2 point1;
		
			// starting drawn point
			glm::vec2 startPoint;

			IntersectionInfo()
			{}

			IntersectionInfo(glm::vec2 intersectionPointIn, glm::vec2 point0In, glm::vec2 point1In, glm::vec2 startPointIn)
			{
				intersectionPoint = intersectionPointIn;
				point0 = point0In;
				point1 = point1In;
				startPoint = startPointIn;
			}
		};



		static float isZero(float num)
		{
			return abs(num) < EPSILON;
		}

		static float cross(glm::vec2 v0, glm::vec2 v1)
		{
			return v0.x * v1.y - v1.x * v0.y;
		}

		void computeLeastSquaresForLineofBestFit();
		void postProcessInputPoints();

		void postProcess();

		// https://stackoverflow.com/questions/563198/whats-the-most-efficent-way-to-calculate-where-two-line-segments-intersect/565282#565282
		// https://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
		bool LineSegmentLineSegmentIntersection(glm::vec2 p0, glm::vec2 p1, glm::vec2 q0, glm::vec2 q1, glm::vec2& intersectionPoint, bool considerCollinearOverlapAsIntersect);


		bool isExistingVertex(glm::vec2 point)
		{
			for (int i = 0; i < vertices.size(); i++)
			{
				if (vertices[i].pos.x == point.x && vertices[i].pos.y == point.y)
				{
					return true;
				}
			}

			return false;
		}

		// find all the intersection caused by this new line(p0, p1)
		vector<IntersectionInfo> getIntersectionList(glm::vec2 p0, glm::vec2 p1);

		void printPointsAndLines();
		void printVerticesAndEdges();

		void processIntersection(IntersectionInfo info);
		void recordNewInputPoint(glm::vec2 point);
		void processNewPoint(glm::vec2 point);
		void doEarClipping();
		bool hasAlreadyThisEdge(int id0, int id1);


		bool getCycleStartingVertex(Vertex& output);

		glm::vec2 perp(glm::vec2 v);
		float perpDot(glm::vec2 v0, glm::vec2 v1);
		bool isCCWFromOrColinear(glm::vec2 v0, glm::vec2 v1);
		bool isCCWFrom(glm::vec2 v0, glm::vec2 v1);
		bool isCWFromOrColinear(glm::vec2 v0, glm::vec2 v1);
		bool isCWFrom(glm::vec2 v0, glm::vec2 v1);

		// mainly used for selecting the first starting node
		bool getClockWiseMostVertex(Vertex vCur, glm::vec2 prevDir, Vertex& output);
		bool getClockWiseMostVertex(Vertex vPrev, Vertex vCur, Vertex& output);
		bool getCounterClockWiseMostVertex(Vertex vPrev, Vertex vCur, Vertex& output);

		void createVerticesAndEdges();
		void findAllMinimalCycleBasis();

		bool isValidPointForLinearFit(int start, int end, float thresholdError);

		void printPolygons();

		int getNumVertices()
		{
			return vertices.size();
		}

		Vertex& getFirstVertex()
		{
			return vertices[0];
		}

		Vertex& getLastVertex()
		{
			return vertices[vertices.size() - 1];
		}

		Vertex& getSecondLastVertex()
		{
			return vertices[vertices.size() - 2];
		}

		void reset()
		{
			inputPoints.clear();			
			points.clear();
			lines.clear();

			vertices.clear();
			edges.clear();
		}


		int getNewVertexId()
		{
			return vertices.size();
		}

		// this is gonna be a base function
		void addVertex(glm::vec2 point, int newId)
		{
			Vertex v = Vertex();
			v.id = newId;
			v.pos = point;
			vertices.push_back(v);
		}

		void addEdge(int id0, int id1)
		{
			Edge e = Edge();
			e.id0 = id0;
			e.id1 = id1;

			edges.push_back(e);
		}


		void addInputPoint(glm::vec2 inputPoint)
		{
			inputPoints.push_back(inputPoint);
		}

		int getNumInputPoints()
		{
			return inputPoints.size();
		}

		glm::vec2 getFirstInputPoint()
		{
			return inputPoints[0];
		}





		int getNumPoints()
		{
			return points.size();
		}

		glm::vec2 getFirstPoint()
		{
			return points[0];
		}


		Object serializePoints(glm::vec2 p);
		glm::vec2 deserializePoints(const mObject& obj);

		void loadInputRawPoints(const mObject& obj);
		void loadRawPoints(const mObject& obj);


		void loadVerticesData(const mObject& obj);
		void loadTestData(char* file);
		void saveTestData();



		void verifyLoadTestDataFunction(vector<Vertex> oriVertices, vector<Edge> oriEdges,
										vector<Vertex> newVertices, vector<Edge> newEdges);

		Object serializeVertex(Vertex v);
		Vertex deserializeVertex(const mObject& obj);
		void determinePolygonsInsideOutside();
		bool windingNumberPointPolygon(glm::vec2 point);
		vector<Vertex> getVerticesByIds(vector<int> vertexIds);

	private:
		void removeEdge(Edge edge);
		bool alreadyInVector(vector<Edge> toBeRemoved, Edge edge);

		void backupVerticesAndEdges();
	

		int hasAlreadyProcessedThisPoint(glm::vec2 point);

		int getPointIndex(glm::vec2 point0, glm::vec2 point1);

		glm::vec2 findPointInsideOfTriangle(vector<Vertex> triangle);



		glm::vec2 getLastPoint()
		{
			return points[points.size() - 1];
		}


		// by traversal order
		vector<glm::vec2> inputPoints;
		vector<glm::vec2> points;
		vector<Line> lines;

		void nestedSubgraphPostprocess(int root, vector<int> closedWalk, int start, int end);
		void processNestedLoops(vector<vector<int>> nestedLoops);
		void processNestedLoop(vector<int> nestedLoop);

	public:
		std::function<void(glm::vec2) > onAddIntersection;
		std::function<void(glm::vec2, glm::vec2)> createLineCallback;
		std::function<void(glm::vec2)> createPointCallback;

		// the list of vertices that forms a polygon/closed loop in your drawing
		vector< vector<int>> polygons;
		vector<bool> polygonInsideFlags;

		vector<EarclippingPolygon> earclippingPolygons;

		vector<Vertex> vertices;
		vector<Edge> edges;


		vector<Edge> m_removedEdges;

		vector<Vertex> backupVertices;
		vector<Edge> backupEdges;
		bool saveLatest;

		int newPointCounter;
};





#endif // ! DRAWING_H_
