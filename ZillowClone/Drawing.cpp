#include "Drawing.h"

float Drawing::EPSILON = 1e-5;

// find all the intersection caused by this new line(p0, p1)
vector<Drawing::IntersectionInfo> Drawing::getIntersectionList(glm::vec2 p0, glm::vec2 p1)
{
	vector<IntersectionInfo> intersectionEdges;
	for (int i = 0; i < lines.size(); i++)
	{
		glm::vec2 intersectionPoint;

		glm::vec2 lp0 = lines[i].p0;
		glm::vec2 lp1 = lines[i].p1;

		utl::debug("p0", p0);
		utl::debug("p1", p1);
		utl::debug("lp0", lp0);
		utl::debug("lp1", lp1);


	//	if (p0 != lp0 && p1 != lp1 || p0 != lp1 && p1 != lp0)
		{
			if (LineSegmentLineSegmentIntersection(p0, p1, lp0, lp1, intersectionPoint, false))
			{
				// we don't consider intersecting the start or end point of where u came from
				if (intersectionPoint != p0 && intersectionPoint != p1)
				{
					IntersectionInfo intersectionInfo(intersectionPoint, lp0, lp1, p0);

					intersectionEdges.push_back(intersectionInfo);
				}
			}
		}
	}

	return intersectionEdges;
}


bool sortByDistToStartPoint(Drawing::IntersectionInfo info0, Drawing::IntersectionInfo info1)
{
	glm::vec2 vec0 = info0.intersectionPoint - info0.startPoint;
	glm::vec2 vec1 = info1.intersectionPoint - info1.startPoint;

	return glm::dot(vec0, vec0) < glm::dot(vec1, vec1);
}




void Drawing::processNewPoint(glm::vec2 point)
{
	cout << "	Processing New Point" << endl;
	if (getNumPoints() > 0)
	{
		glm::vec2 lastPoint = getLastPoint();
		vector<IntersectionInfo> intersections = getIntersectionList(lastPoint, point);

		// sort intersection by distance to starting point
		std::sort(intersections.begin(), intersections.end(), sortByDistToStartPoint);

		Line tempLine;

		cout << "	intersections.size() " << intersections.size() << endl;

		for (int i = 0; i < intersections.size(); i++)
		{
			IntersectionInfo info = intersections[i];
			processIntersection(info);

			// then also add the segement 
			tempLine = Line(lastPoint, info.intersectionPoint);
			lines.push_back(tempLine);

			int index = getPointIndex(info.point0);
			points.insert(points.begin() + index + 1, info.intersectionPoint);

			lastPoint = info.intersectionPoint;
			points.push_back(info.intersectionPoint);

			if (onAddIntersection != NULL)
			{
				onAddIntersection(info.intersectionPoint);
			}
		}

		tempLine = Line(lastPoint, point);
		lines.push_back(tempLine);

		points.push_back(point);
	}
	else
	{
		points.push_back(point);
	}

	cout << "	*****" << endl;
	cout << "	Printing points" << endl;
	for (int i = 0; i < points.size(); i++)
	{
		cout << "		point is " << points[i].x << " " << points[i].y << endl;
	}

	cout << "	*****" << endl;
	cout << "	Print lines" << endl;
	for (int i = 0; i < lines.size(); i++)
	{
		lines[i].print();
	}

}

void Drawing::processIntersection(IntersectionInfo info)
{
	// remove previous line
	for (int j = 0; j < lines.size(); j++)
	{
		if (lines[j].p0 == info.point0 && lines[j].p1 == info.point1)
		{
			lines.erase(lines.begin() + j);
		}
	}

	// adding the split line segments
	Line line0 = Line(info.point0, info.intersectionPoint);
	Line line1 = Line(info.intersectionPoint, info.point1);

	lines.push_back(line0);
	lines.push_back(line1);

}


void Drawing::postProcess()
{
	cout << "############ In Post Process " << points.size() << " " << endl;


	for (int i = 0; i < points.size(); i++)
	{
		cout << "		" << points[i].x << " " << points[i].y << endl;
	}

	cout << "Actually processing " << endl;
	glm::vec2 lastPoint;

	for (int i = 0; i < points.size(); i++)
	{
		if (i == 0)
		{
			int newId = getNewVertexId();
			addVertex(points[i], newId);
		}
		else
		{
			int index0 = hasAlreadyProcessedThisPoint(points[i - 1]);
			int index1 = hasAlreadyProcessedThisPoint(points[i]);

			Vertex v0 = vertices[index0];
			Vertex v1;

			if (index1 != -1)
			{
				v1 = vertices[index1];
			}
			else
			{
				int newId = getNewVertexId();
				addVertex(points[i], newId);
				v1 = vertices[vertices.size() - 1];
			}

			if (hasAlreadyThisEdge(v0.id, v1.id) == false)
			{
				// or you can do a check and see if the are any existing edges already
				addEdge(v0.id, v1.id);

				v0.addNeighbor(v1.id);
				v1.addNeighbor(v0.id);
			}
		}
	}

	cout << " printing vertices " << vertices.size() << endl;
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].print();
	}


	cout <<endl << endl << " printing edges" << edges.size() << endl;
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i].print();
	}

}


int Drawing::hasAlreadyProcessedThisPoint(glm::vec2 point)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].coord.x == point.x && vertices[i].coord.y == point.y)
		{
			return i;
		}
	}

	return -1;
}


bool Drawing::hasAlreadyThisEdge(int id0, int id1)
{
	for (int i = 0; i < edges.size(); i++)
	{
		if (edges[i].id0 == id0 && edges[i].id1 == id1 || 
			edges[i].id1 == id0 && edges[i].id0 == id1)
		{
			return true;
		}
	}

	return false;
}

int Drawing::getPointIndex(glm::vec2 point)
{
	for (int i = 0; i < points.size(); i++)
	{
		if (points[i].x == point.x && points[i].y == point.y)
		{
			return i;
		}
	}

	return -1;
}



// https://stackoverflow.com/questions/563198/whats-the-most-efficent-way-to-calculate-where-two-line-segments-intersect/565282#565282
// https://www.codeproject.com/Tips/862988/Find-the-Intersection-Point-of-Two-Line-Segments
bool Drawing::LineSegmentLineSegmentIntersection(glm::vec2 p0, glm::vec2 p1, glm::vec2 q0, glm::vec2 q1, glm::vec2& intersectionPoint, bool considerCollinearOverlapAsIntersect)
{
	glm::vec2 r = p1 - p0;
	glm::vec2 s = q1 - q0;

	float rxs = cross(r, s);
	float qpxr = cross(q0 - p0, r);
	float qpxs = cross(q0 - p0, s);

	// if r x s = 0 and (q-p) x r = 0, then the two lines are collinear
	if (isZero(rxs) && isZero(qpxr))
	{
		if (considerCollinearOverlapAsIntersect)
		{
			float qpdotr = glm::dot(q0 - p0, r);
			float rdotr = glm::dot(r, r);

			float qpdots = glm::dot(p0 - q0, s);
			float sdots = glm::dot(s, s);

			if ((0 <= qpdotr && qpdotr <= rdotr) ||
				(0 <= qpdots && qpdots <= sdots))
			{
				return true;
			}
		}
		return false;
	}

	if (isZero(rxs) && !isZero(qpxr))
	{
		return false;
	}

	float t = qpxs / rxs;
	float u = qpxr / rxs;

	if (!isZero(rxs) && (0 <= t && t <= 1) && (0 <= u && u <= 1))
	{
		intersectionPoint = p0 + r * t;
		return true;
	}

	return false;

}
