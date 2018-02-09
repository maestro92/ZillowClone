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

		if (LineSegmentLineSegmentIntersection(p0, p1, lp0, lp1, intersectionPoint, true))
		{
			// might intersect with two lines at the same point,
			// we'll worry about that later
			if (intersectionPoint.x != p0.x || intersectionPoint.y != p0.y)
			{
				IntersectionInfo intersectionInfo(intersectionPoint, lp0, lp1, p0);

				intersectionEdges.push_back(intersectionInfo);
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

		cout << "intersections.size() " << intersections.size() << endl;

		for (int i = 0; i < intersections.size(); i++)
		{
			IntersectionInfo info = intersections[i];
			processIntersection(info);

			// then also add the segement 
			tempLine = Line(lastPoint, info.intersectionPoint);
			lines.push_back(tempLine);
			points.push_back(info.intersectionPoint);
			lastPoint = info.intersectionPoint;

			if (onAddIntersection != NULL)
			{
				onAddIntersection(info.intersectionPoint);
			}
		}

		tempLine = Line(lastPoint, point);
		lines.push_back(tempLine);

		points.push_back(point);

		cout << "Printing points" << endl;
		for (int i = 0; i < points.size(); i++)
		{
			cout << "	point is " << points[i].x << " " << points[i].y << endl;
		}

		cout << "Print lines" << endl;
		for (int i = 0; i < lines.size(); i++)
		{
			lines[i].print();
		}

	}
	else
	{
		points.push_back(point);

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

	Line line0 = Line(info.point0, info.intersectionPoint);
	Line line1 = Line(info.intersectionPoint, info.point1);

	lines.push_back(line0);
	lines.push_back(line1);

}


void Drawing::postProcess()
{
	cout << "############ In Post Process " << points.size() << " " << vertices.size() << endl;
	for (int i = 0; i < points.size(); i++)
	{
		int index = hasAlreadyProcessedThisPoint(points[i]);
		cout << "		" << points[i].x << " " << points[i].y << endl;
		if (index != -1)
		{
			Vertex v1 = vertices[index];
			Vertex v0 = getLastVertex();
			cout <<"2nd place Edge: " << v0.id << " " << v1.id << endl;
			addEdge(v0.id, v1.id);
			v0.addNeighbor(v1.id);
			v1.addNeighbor(v0.id);
		}
		else
		{
			int newId = getNewVertexId();

			addVertex(points[i], newId);
			if (getNumVertices() > 1)
			{
				Vertex v0 = getSecondLastVertex();	
				Vertex v1 = getLastVertex();
				
				cout << "Edge: " << v0.id << " " << v1.id << endl;
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
