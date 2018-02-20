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

	//	if (p0 != lp0 && p1 != lp1 || p0 != lp1 && p1 != lp0)
		{
			bool flag = LineSegmentLineSegmentIntersection(p0, p1, lp0, lp1, intersectionPoint, false);
			if (flag)
			{
				// we don't consider intersecting the start or end point of where u came from
				if (!utl::equals(intersectionPoint, p0) && !utl::equals(intersectionPoint, p1))
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
//	utl::debug(">>>>>>>>> Adding New Point", point);
	if (getNumPoints() > 0)
	{
		glm::vec2 lastPoint = getLastPoint();
		vector<IntersectionInfo> intersections = getIntersectionList(lastPoint, point);

		// sort intersection by distance to starting point
		std::sort(intersections.begin(), intersections.end(), sortByDistToStartPoint);

		Line tempLine;

		/*
		cout << "before" << endl;
		for (int i = 0; i < points.size(); i++)
		{
			cout << i << "		" << points[i].x << " " << points[i].y << endl;
		}
		*/

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

		/*
		cout << "After" << endl;
		for (int i = 0; i < points.size(); i++)
		{
			cout << i << "		" << points[i].x << " " << points[i].y << endl;
		}
		*/

	}
	else
	{
		points.push_back(point);
	}

	/*
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
	*/
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

// implement the algorithm in this post
// https://stackoverflow.com/questions/16782898/algorithm-for-finding-minimal-cycles-in-a-graph

/*
void Drawing::calculateVerticesAngle()
{
	for(int i=0;
}
*/

void Drawing::createVerticesAndEdges()
{
	/*
	cout << "############ In Post Process " << points.size() << " " << endl;

	for (int i = 0; i < points.size(); i++)
	{
		cout << i <<	"		" << points[i].x << " " << points[i].y << endl;
	}


	cout << "Actually processing " << endl;
	*/
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
			int id0 = hasAlreadyProcessedThisPoint(points[i - 1]);
			int id1 = hasAlreadyProcessedThisPoint(points[i]);

			if (id1 == -1)
			{
				int newId = getNewVertexId();
				addVertex(points[i], newId);
				id1 = newId;
			}

			if (hasAlreadyThisEdge(id0, id1) == false)
			{
				// or you can do a check and see if the are any existing edges already
				addEdge(id0, id1);

				vertices[id0].addNeighbor(id1);
				vertices[id1].addNeighbor(id0);
			}
		}
	}


	/*
	cout << " printing vertices " << vertices.size() << endl;
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].print();
	}

	cout << endl << endl << " printing edges" << edges.size() << endl;
	for (int i = 0; i < edges.size(); i++)
	{
		edges[i].print();
	}
	*/
}

// 4.1, he had a definition for convex
// D · D0(perpendicular sign) < 0 and D · D1(perpendicular sign) > 0
// if you think about it in terms of 3D, try to do a right hand rule that setup and you'll see why this definition works to 
// find out if something is convex or concave


/*
https://stackoverflow.com/questions/1119627/how-to-test-if-a-point-is-inside-of-a-convex-polygon-in-2d-integer-coordinates

GPU Gems Chapter II.5, it does a very good job explaining how the perp-dot operator works.
http://cas.xav.free.fr/Graphics%20Gems%204%20-%20Paul%20S.%20Heckbert.pdf

The first thing we want to know is if the vector to the adjacent vertex, are CW or CCW
the Perp operator helps to determine if a vector, relative to another vector is CW or CCW

the line 

here we see if D_next is CW relative to D_curr
*/


// https://www.geometrictools.com/Documentation/MinimalCycleBasis.pdf
// this is part 4 of the pdf
void Drawing::findAllMinimalCycleBasis()
{
	polygons.clear();

	backupVerticesAndEdges();

	glm::vec2 supportLine = glm::vec2(0, -1);
	Vertex startVertex;
	bool hasValidStartVertex = getCycleStartingVertex(startVertex);


	// we wish to keep the interior immediately to the left when we traverse through V1
	vector<Edge> cycleEdgeList;

	int k = 0;
	int iterations = 0;
	while (hasValidStartVertex)
	{
		bool started = false;
		Vertex vCurr = startVertex;
		Vertex vPrev;
		hasValidStartVertex = false;
		iterations = 0;

	//	cout << "		Starting new cycle with " << startVertex.id << endl;

		cycleEdgeList.clear();
		while (vCurr != startVertex || started == false)
		{
			Vertex vNext;
			if (started == false)
			{
				hasValidStartVertex = getClockWiseMostVertex(startVertex, supportLine, vNext);
				started = true;
			}
			else
			{
				hasValidStartVertex = getCounterClockWiseMostVertex(vPrev, vCurr, vNext);
			}

			if (hasValidStartVertex == false)
			{
				cout << "Shouldn't Be here!!!!!!!!!!!!!!!";
				break;
			}

			vPrev = vCurr;
			vCurr = vNext;

			Edge edge(vPrev.id, vCurr.id);
			cycleEdgeList.push_back(edge);
		}

		vector<int> tempList;
		for (int i = 0; i < cycleEdgeList.size(); i++)
		{			
			if (i == 0)
			{
				tempList.push_back(cycleEdgeList[i].id0);
				tempList.push_back(cycleEdgeList[i].id1);
			}
			else			
			{
				tempList.push_back(cycleEdgeList[i].id1);
			}
		}

		polygons.push_back(tempList);


		// first to CCW traversal to remove edges
		vector<Edge> toBeRemoved;
		// remove removable edges
		for (int i = 0; i < cycleEdgeList.size(); i++)
		{
			Vertex v = vertices[cycleEdgeList[i].id1];
			toBeRemoved.push_back(cycleEdgeList[i]);

			if (v.neighbors.size() > 2)
			{		
				break;
			}
		}


		// then to CW traversal to remove edges
		for (int i = cycleEdgeList.size() - 1; i >= 0; i--)
		{
			Vertex v = vertices[cycleEdgeList[i].id0];
			if (alreadyInVector(toBeRemoved, cycleEdgeList[i]) == false)
			{
				toBeRemoved.push_back(cycleEdgeList[i]);
			}

			if (v.neighbors.size() > 2)
			{
				break;
			}			
		}

		for (int i = 0; i < toBeRemoved.size(); i++)
		{
			removeEdge(toBeRemoved[i]);
		}

		hasValidStartVertex = getCycleStartingVertex(startVertex);
		k++;
	}



	cout << "Printing VerticesGroups" << endl;
	for (int i = 0; i < polygons.size(); i++)
	{
		vector<Vertex> unprocessedPolygonVertices;

		for (int j = 0; j < polygons[i].size(); j++)
		{
			cout << polygons[i][j] << " ";
			int id = polygons[i][j];
			Vertex newV = Vertex(vertices[id]);

			newV.resetNeighbors();
			unprocessedPolygonVertices.push_back(newV);
		}
		cout << endl;

		EarclippingPolygon earclippingPolygon;
		earclippingPolygon.initFromUnprocessedVertices(unprocessedPolygonVertices);

		earclippingPolygons.push_back(earclippingPolygon);
	}

	cout << "############ Printing earclippingPolygons" << endl;
	for (int i = 0; i < earclippingPolygons.size(); i++)
	{
		cout << "		Printing earclippingPolygon" << endl;
		earclippingPolygons[i].print();
	}
}




bool Drawing::alreadyInVector(vector<Edge> toBeRemoved, Edge edge)
{
	for (int i = 0; i < toBeRemoved.size(); i++)
	{
		if (toBeRemoved[i] == edge)
		{
			return true;
		}
	}
	return false;
}


void Drawing::loadTestData(char* filename)
{
	mValue content = utl::readJsonFileToMap(filename);

	const mArray& addr_array = content.get_array();

	vector<Vertex> newVertices;
	vector<Edge> newEdges;
	for (int i = 0; i < addr_array.size(); i++)
	{
		const mObject obj = addr_array[i].get_obj();
		Vertex newV = deserializeVertex(obj);
		newVertices.push_back(newV);		
	}

	vertices = newVertices;
	edges = newEdges;

//	verifyLoadTestDataFunction(vertices, edges, newVertices, newEdges);
}

Vertex Drawing::deserializeVertex(const mObject& obj)
{
	Vertex v;
	v.id = utl::findValue(obj, "id").get_int();
	float x = utl::findValue(obj, "x").get_real();
	float y = utl::findValue(obj, "y").get_real();

	glm::vec2 pos = glm::vec2(x, y);

	v.pos = pos;

	const mArray& neighborsArray = utl::findValue(obj, "neighbors").get_array();
	for (int j = 0; j < neighborsArray.size(); j++)
	{
		int neighborId = neighborsArray[j].get_int();
		v.neighbors.push_back(neighborId);
	}
	return v;
}


void Drawing::verifyLoadTestDataFunction(vector<Vertex> oriVertices, vector<Edge> oriEdges,
								vector<Vertex> newVertices, vector<Edge> newEdges)
{
	
	cout << "verifyLoadTestDataFunction " << oriVertices.size() << " " << newVertices.size() << endl;
	if (oriVertices.size() != newVertices.size())
	{
		string excceptionMsg = "Size Don't Match";
		excceptionMsg += "orgVertices has size " + utl::intToStr(oriVertices.size());
		excceptionMsg += ", newVertices has size " + utl::intToStr(newVertices.size());
		throw string(excceptionMsg);
	}


	for (int i = 0; i < newVertices.size(); i++)
	{
		Vertex v0 = oriVertices[i];
		Vertex v1 = newVertices[i];

		if (v0.id != v1.id)
		{
			string excceptionMsg = "Id Don't Match on id " + utl::floatToStr(v0.id);
			throw string(excceptionMsg);
		}
		else if (v0.pos != v1.pos)
		{
			string excceptionMsg = "position Don't Match " + utl::vec2ToStr(v0.pos) + " != " + utl::vec2ToStr(v1.pos);
			throw string(excceptionMsg);
		}

		v0.print();
		v1.print();

	}
}


Object Drawing::serializeVertex(Vertex v)
{
	Object vertexObj;
	const Value neighbors(v.neighbors.begin(), v.neighbors.end());
	
	vertexObj.push_back(Pair("id", v.id));
	vertexObj.push_back(Pair("x", v.pos.x));
	vertexObj.push_back(Pair("y", v.pos.y));
	vertexObj.push_back(Pair("neighbors", neighbors));

	return vertexObj;
}


void Drawing::saveTestData()
{
	ofstream myfile;
	myfile.open("data.txt");

	Array verticesArray;

	for (int i = 0; i < vertices.size(); i++)
	{
		Object vObj = serializeVertex(vertices[i]);
		verticesArray.push_back(vObj);
	}

	write(verticesArray, myfile, pretty_print);
	myfile.close();
}



void Drawing::removeEdge(Edge edge)
{
	vertices[edge.id0].removeNeighbor(edge.id1);
	vertices[edge.id1].removeNeighbor(edge.id0);
}


void Drawing::backupVerticesAndEdges()
{
	// C++ vector does deep copy
	// https://stackoverflow.com/questions/11348376/std-vector-c-deep-or-shallow-copy
	backupVertices = vertices;
	backupEdges = edges;
}


// we select the Vertex that has minimum x-value. If there are two or more vertices attaining the minimum x-value,
// select V0 to be the vertex that additionally has minimum y-value-
bool Drawing::getCycleStartingVertex(Vertex& output)
{
	if (vertices.size() == 0)
	{
		output = Vertex();
		return false;
	}

	Vertex startNode;
	for (int i = 0; i < vertices.size(); i++)
	{
	//	cout << vertices[i].id << ", neighbor size is " << vertices[i].neighbors.size() << endl;
		if (vertices[i].neighbors.size() > 0)
		{
			if (!startNode.isValid())
			{
				startNode = vertices[i];
			}
			else
			{
				if (vertices[i].pos.x <= startNode.pos.x)
				{
					if (vertices[i].pos.x == startNode.pos.x)
					{
						if (vertices[i].pos.y < startNode.pos.y)
						{
							startNode = vertices[i];
						}
					}
					else
					{
						startNode = vertices[i];
					}
				}
			}
		}
	}

	output = startNode;
	return startNode.isValid();
}



// http://cas.xav.free.fr/Graphics%20Gems%204%20-%20Paul%20S.%20Heckbert.pdf
// page 140
// the picture I always refer to is Figure2 of that article
// this gives perp(v1) * (v0)  
float Drawing::perpDot(glm::vec2 v0, glm::vec2 v1)
{
	v0 = perp(v0);
	return glm::dot(v0, v1);
//	return v0.x * v1.y - v0.y * v1.x;
}

glm::vec2 Drawing::perp(glm::vec2 v)
{
/*
	float temp = v.x;
	v.x = -v.y;
	v.y = temp;
*/
	return glm::vec2(-v.y, v.x);
}


// returns whether v0 is CCW from v1;
bool Drawing::isCCWFrom(glm::vec2 v0, glm::vec2 v1)
{
	return perpDot(v1, v0) > 0;
}

// returns whether v0 is CCW or Colinear from v1;
bool Drawing::isCCWFromOrColinear(glm::vec2 v0, glm::vec2 v1)
{
	return perpDot(v1, v0) >= 0;
}

// returns whether v0 is CW of v1;
bool Drawing::isCWFrom(glm::vec2 v0, glm::vec2 v1)
{
	return perpDot(v1, v0) < 0;
}

// returns whether v0 is CW of v1;
bool Drawing::isCWFromOrColinear(glm::vec2 v0, glm::vec2 v1)
{
	return perpDot(v1, v0) <= 0;
}


// same as the getClockWiseMost Vertex
bool Drawing::getClockWiseMostVertex(Vertex vPrev, Vertex vCur, Vertex& output)
{
	if(vCur.neighbors.size() == 0)
	{
		output = Vertex();
		return false;
	}

	int firstValidNeighbor = vCur.getFirstNeighborThatIsNot(vPrev.id);
	if (firstValidNeighbor == -1)
	{
		output = Vertex();
		return false;
	}

	glm::vec2 prevDir = vCur.pos - vPrev.pos;
	Vertex vNext = vertices[firstValidNeighbor];
	glm::vec2 curBestDir = vNext.pos - vCur.pos;

	bool curBestDirIsCWFromPrevDirFlag = isCWFromOrColinear(curBestDir, prevDir);

	// first found adjacent vertex that is not prev is chosen as V_next
	for (int i = 1; i < vCur.neighbors.size(); i++)
	{
		int neighborId = vCur.neighbors[i];
		if (neighborId == vPrev.id)
		{
			continue;
		}
		Vertex vNeighbor = vertices[neighborId];
		glm::vec2 newDir = vNeighbor.pos - vCur.pos;

		if (curBestDirIsCWFromPrevDirFlag)
		{
			if (isCWFrom(newDir, prevDir) && isCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCWFromPrevDirFlag = isCWFromOrColinear(curBestDir, prevDir);
			}
		}
		else
		{
			if (isCWFrom(newDir, prevDir) || isCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCWFromPrevDirFlag = isCWFromOrColinear(curBestDir, prevDir);
			}
		}
	}

	output = vNext;
	return true;
}



// same as the getClockWiseMost Vertex
bool Drawing::getClockWiseMostVertex(Vertex vCur, glm::vec2 prevDir, Vertex& output)
{
	if (vCur.neighbors.size() == 0)
	{
		output = Vertex();
		return false;
	}

	Vertex vNext = vertices[vCur.neighbors[0]];
	glm::vec2 curBestDir = vNext.pos - vCur.pos;

	bool curBestDirIsCWFromPrevDirFlag = isCWFromOrColinear(curBestDir, prevDir);
	int neighborId2 = vCur.neighbors[0];
	/*
	cout << "neighborId " << neighborId2 << endl;
	cout << "		curBestDirIsCWFromPrevDirFlag " << curBestDirIsCWFromPrevDirFlag << endl;
	cout << "			curBestDir " << curBestDir.x << " " << curBestDir.y << endl;
	cout << "			prevDir " << prevDir.x << " " << prevDir.y << endl;
	*/

	// first found adjacent vertex that is not prev is chosen as V_next
	for (int i = 1; i < vCur.neighbors.size(); i++)
	{
		int neighborId = vCur.neighbors[i];
//		cout << "neighborId " << neighborId << endl;
		Vertex vNeighbor = vertices[neighborId];
		glm::vec2 newDir = vNeighbor.pos - vCur.pos;

		/*
		cout << "		curBestDirIsCWFromPrevDirFlag " << curBestDirIsCWFromPrevDirFlag << endl;
		cout << "			curBestDir " << curBestDir.x << " " << curBestDir.y << endl;
		cout << "			newDir " << newDir.x << " " << newDir.y << endl;
		cout << "			prevDir " << prevDir.x << " " << prevDir.y << endl;
		*/
		if (curBestDirIsCWFromPrevDirFlag)
		{
			if (isCWFrom(newDir, prevDir) && isCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCWFromPrevDirFlag = isCWFromOrColinear(curBestDir, prevDir);
			}
		}
		else
		{
			if (isCWFrom(newDir, prevDir) || isCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCWFromPrevDirFlag = isCWFromOrColinear(curBestDir, prevDir);
			}
		}
	}

	output = vNext;
	return true;
}


// same as the getCounterClockWiseMost Vertex
bool Drawing::getCounterClockWiseMostVertex(Vertex vPrev, Vertex vCur, Vertex& output)
{
	if (vCur.neighbors.size() == 0)
	{
		output = Vertex();
		return false;
	}

	int firstValidNeighbor = vCur.getFirstNeighborThatIsNot(vPrev.id);
	if (firstValidNeighbor == -1)
	{
		output = Vertex();
		return false;
	}

	glm::vec2 prevDir = vCur.pos - vPrev.pos;
	Vertex vNext = vertices[firstValidNeighbor];
	glm::vec2 curBestDir = vNext.pos - vCur.pos;

	bool curBestDirIsCCWFromPrevDirFlag = isCCWFromOrColinear(curBestDir, prevDir);

	int neighborId2 = vCur.neighbors[0];
/*
	cout << "neighborId " << neighborId2 << endl;
	cout << "		curBestDirIsCWFromPrevDirFlag " << curBestDirIsCCWFromPrevDirFlag << endl;
	cout << "			curBestDir " << curBestDir.x << " " << curBestDir.y << endl;
	cout << "			prevDir " << prevDir.x << " " << prevDir.y << endl;
	*/


	// first found adjacent vertex that is not prev is chosen as V_next
	for (int i = 1; i < vCur.neighbors.size(); i++)
	{		
		int neighborId = vCur.neighbors[i];
//		cout << "neighborId " << neighborId << endl;

		if (neighborId == vPrev.id)
		{
			continue;
		}
		
		Vertex vNeighbor = vertices[neighborId];
		glm::vec2 newDir = vNeighbor.pos - vCur.pos;
		/*
		cout << "		curBestDirIsCWFromPrevDirFlag " << curBestDirIsCCWFromPrevDirFlag << endl;
		cout << "			curBestDir " << curBestDir.x << " " << curBestDir.y << endl;
		cout << "			newDir " << newDir.x << " " << newDir.y << endl;
		cout << "			prevDir " << prevDir.x << " " << prevDir.y << endl;
		*/
		if (curBestDirIsCCWFromPrevDirFlag)
		{
			if (isCCWFrom(newDir, prevDir) && isCCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCCWFromPrevDirFlag = isCCWFromOrColinear(curBestDir, prevDir);
			}
		}
		else
		{
			if (isCCWFrom(newDir, prevDir) || isCCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCCWFromPrevDirFlag = isCCWFromOrColinear(curBestDir, prevDir);
			}
		}
	}

	output = vNext;
	return true;
}



void Drawing::postProcess()
{
	createVerticesAndEdges();

	cout << " ############### postProcess" << endl;

	if (saveLatest)
	{
		saveTestData();
	}

	findAllMinimalCycleBasis();
}


int Drawing::hasAlreadyProcessedThisPoint(glm::vec2 point)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (vertices[i].pos.x == point.x && vertices[i].pos.y == point.y)
		{
			return vertices[i].id;
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
