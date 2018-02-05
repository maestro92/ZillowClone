#ifndef  DRAWING_H_
#define DRAWING_H_

#include "graph_structs.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector
#include <functional>
#include "utility.h"
#include "utility_debug.h"

class Drawing
{
	public:

		static float EPSILON;

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


		void processDrawingOnBtnMouseUp()
		{

		}


		static float isZero(float num)
		{
			return abs(num) < EPSILON;
		}

		static float cross(glm::vec2 v0, glm::vec2 v1)
		{
			return v0.x * v1.y - v1.x * v0.y;
		}


		// https://stackoverflow.com/questions/563198/whats-the-most-efficent-way-to-calculate-where-two-line-segments-intersect/565282#565282
		// https://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
		bool LineSegmentLineSegmentIntersection(glm::vec2 p0, glm::vec2 p1, glm::vec2 q0, glm::vec2 q1, glm::vec2& intersectionPoint, bool considerCollinearOverlapAsIntersect);


		bool isExistingVertex(glm::vec2 point)
		{
			for (int i = 0; i < vertices.size(); i++)
			{
				if (vertices[i].coord.x == point.x && vertices[i].coord.y == point.y)
				{
					return true;
				}
			}

			return false;
		}

		// find all the intersection caused by this new line(p0, p1)
		vector<IntersectionInfo> getIntersectionList(glm::vec2 p0, glm::vec2 p1);


		void processIntersection(IntersectionInfo info);

		void processNewPoint(glm::vec2 point);

		int getNumVertices()
		{
			return vertices.size();
		}

		Vertex getFirstVertex()
		{
			return vertices[0];
		}

		Vertex getLastVertex()
		{
			return vertices[vertices.size() - 1];
		}

		void reset()
		{
			points.clear();
			lines.clear();

			vertices.clear();
			edges.clear();
		}


		int getNewId()
		{
			return vertices.size();
		}

		// this is gonna be a base function
		void addVertex(glm::vec2 point, int newId)
		{
			Vertex v = Vertex();
			v.id = newId;
			v.coord = point;
			vertices.push_back(v);
		}

		void addEdge(int id0, int id1)
		{
			Edge e = Edge();
			e.id0 = id0;
			e.id1 = id1;

			edges.push_back(e);
		}


		int getNumPoints()
		{
			return points.size();
		}


	private:


		glm::vec2 getFirstPoint()
		{
			return points[0];
		}

		glm::vec2 getLastPoint()
		{
			return points[points.size() - 1];
		}


		// by traversal order
		vector<glm::vec2> points;
		vector<Line> lines;

	public:
		std::function<void(glm::vec2) > onAddIntersection;

		vector<Vertex> vertices;
		vector<Edge> edges;
};





#endif // ! DRAWING_H_