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

void Drawing::recordNewInputPoint(glm::vec2 point)
{
	inputPoints.push_back(point);
}

void Drawing::postProcessInputPoints()
{
	points.clear();

	// we don't process the last point
	bool startNewLine = true;
	int start = 0;
	vector<glm::vec2> tempPoints;
	for (int i = 0; i < inputPoints.size(); i++)
	{
		if (i==0 || i == inputPoints.size() - 1)
		{
			tempPoints.push_back(inputPoints[i]);
		}
		else
		{
			float distance = glm::distance2(inputPoints[start], inputPoints[i]);
			float threshHold = distance * 0.001;

			if (isValidPointForLinearFit(start, i, threshHold))
			{
				if (i == inputPoints.size() - 2)
				{
					tempPoints.push_back(inputPoints[i]);
				}
				else
				{
					continue;
				}
			}
			else
			{
				tempPoints.push_back(inputPoints[i]);
				start = i;
			}
		}
	}

	/*
	glm::vec2 p0 = glm::vec2(0);
	glm::vec2 p1 = glm::vec2(0);
	for (int i = 0; i < getNumPoints(); i++)
	{
		if (i == 0)
		{
			// do nothing for now
		}
		else
		{
			p0 = points[i - 1];
			p1 = points[i];

			createLineCallback(p0, p1);
		}

		createPointCallback(points[i]);
	}
	*/

	for (int i = 0; i < tempPoints.size(); i++)
	{
		processNewPoint(tempPoints[i]);
	}


	glm::vec2 p0 = glm::vec2(0);
	glm::vec2 p1 = glm::vec2(0);
	for (int i = 0; i < points.size(); i++)
	{
		if (i == 0)
		{
			// do nothing for now
		}
		else
		{
			p0 = points[i - 1];
			p1 = points[i];

			createLineCallback(p0, p1);
		}

		createPointCallback(points[i]);
	}
}


bool Drawing::isValidPointForLinearFit(int start, int end, float thresholdError)
{
//	float error = 0;
	for (int i = start+1; i <= end-1; i++)
	{		
		float error = utl::sqDistBetweenPointAndLineSetment(inputPoints[start], inputPoints[end], inputPoints[i]);

		if (error > thresholdError)
		{
			return false;
		}
		
	}
	return true;
}

void Drawing::processNewPoint(glm::vec2 point)
{
//	utl::debug(">>>>>>>>> Adding New Point", point);

	if (saveLatest)
	{
		recordNewInputPoint(point);
	}

	if (getNumPoints() > 0)
	{
		glm::vec2 lastPoint = getLastPoint();
		vector<IntersectionInfo> intersections = getIntersectionList(lastPoint, point);

		// sort intersection by distance to starting point
		std::sort(intersections.begin(), intersections.end(), sortByDistToStartPoint);

		Line tempLine;


		for (int i = 0; i < intersections.size(); i++)
		{
			IntersectionInfo info = intersections[i];

			processIntersection(info);

			// then also add the segement 
			tempLine = Line(lastPoint, info.intersectionPoint);
			lines.push_back(tempLine);

			int index = getPointIndex(info.point0, info.point1);

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



	newPointCounter++;

}


void Drawing::printPointsAndLines()
{	
	cout << "	*****" << endl;
	cout << "	Printing points " << points.size() << endl;
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

	// printVerticesAndEdges();
}


void Drawing::printVerticesAndEdges()
{

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

}


// 4.1, he had a definition for convex
// D � D0(perpendicular sign) < 0 and D � D1(perpendicular sign) > 0
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


	int iterations = 0;
	while (hasValidStartVertex)
	{
		bool started = false;
		Vertex vCurr = startVertex;
		Vertex vPrev;
		hasValidStartVertex = false;
		iterations = 0;

	
		cycleEdgeList.clear();
		vector<int> closedWalk;

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
			closedWalk.push_back(vCurr.id);

			vPrev = vCurr;
			vCurr = vNext;

		}


		closedWalk.push_back(startVertex.id);


		// first int is the vertexId, second int is the indicies into the cycleEdgeList;
		unordered_map<int, int> duplicates;
		int nestedLoopStart = -1;
		int nestedLoopEnd = -1;


		
		vector<vector<int>> nestedLoops;
		// one closed walk might have two nested loops
		for (int i = 1; i < closedWalk.size() - 1; ++i)
		{
			// if this is a duplicate
			if (duplicates.find(closedWalk[i]) != duplicates.end())
			{
				nestedLoopStart = duplicates[closedWalk[i]];
				nestedLoopEnd = i;

				vector<int> nestedLoop;
				nestedLoop.push_back(closedWalk[i]);
				for (int j = nestedLoopStart + 1; j < nestedLoopEnd+1; ++j)
				{
					duplicates.erase(closedWalk[j]);
					nestedLoop.push_back(closedWalk[j]);
				}
				nestedLoops.push_back(nestedLoop);

				closedWalk.erase(closedWalk.begin() + nestedLoopStart + 1, 
								 closedWalk.begin() + nestedLoopEnd + 1);
				i = nestedLoopStart;
			}
			else
			{
				duplicates[closedWalk[i]] = i;
			}
		}


		processNestedLoops(nestedLoops);



		vector<Edge> toBeRemoved;

		// for the case if our starting Vertex is a filament edge rather than a cycle edge
		// part 4.3.3 of the paper
		if (closedWalk.size() >= 4)
		{

			for (int i = 0; i < closedWalk.size() - 1; i++)
			{
				int id0 = 0;
				int id1 = 1;
				Edge edge(closedWalk[i], closedWalk[i + 1]);
				cycleEdgeList.push_back(edge);
			}

			//	printVerticesAndEdges();

			polygons.push_back(closedWalk);



			// first to CCW traversal to remove edges
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
				m_removedEdges.push_back(toBeRemoved[i]);
			}
			toBeRemoved.clear();

			// also need to filaments
			// remove nodes that only has one neighbor. this occurs when you gradually remove edges during your findingMinimalCycles steps
			// this needs to happen after removing the cycleEdges



		}
		
		else
		{
			Edge edge;
			edge.id0 = closedWalk[0];
			edge.id1 = closedWalk[1];
			removeEdge(edge);
			m_removedEdges.push_back(edge);
		}
		


		for (int i = 0; i < vertices.size(); i++)
		{
			if (vertices[i].neighbors.size() == 1)
			{
				Edge edge;
				edge.id0 = vertices[i].id;
				edge.id1 = vertices[i].neighbors[0];
				toBeRemoved.push_back(edge);
			}
		}


		// sometimes when you remove one edge, it will make another vertex a filaments
		// so we have to keep on scanning until there are no more filaments
		while (toBeRemoved.size() > 0)
		{
			int id0 = toBeRemoved[0].id1;
			int id1 = toBeRemoved[0].id1;
			removeEdge(toBeRemoved[0]);
			m_removedEdges.push_back(toBeRemoved[0]);
			toBeRemoved.erase(toBeRemoved.begin());

			// push the new one 
			if (vertices[id1].neighbors.size() == 1)
			{
				Edge edge;
				edge.id0 = vertices[id1].id;
				edge.id1 = vertices[id1].neighbors[0];
				toBeRemoved.push_back(edge);
			}
		}


		hasValidStartVertex = getCycleStartingVertex(startVertex);
		iterations++;

	}

//	printPolygons();
	
//	doEarClipping();
}




void Drawing::processNestedLoops(vector<vector<int>> nestedLoops)
{	
//	cout << ">>>>>>> nestedSubgraphPostprocess " << endl;
	for (int i = 0; i < nestedLoops.size(); i++)
	{
		processNestedLoop(nestedLoops[i]);
	}
}


void Drawing::processNestedLoop(vector<int> nestedLoop)
{
	vector<int> neighborList;
	int root = nestedLoop[0];

	for (int i = 0; i < nestedLoop.size(); i++)
	{
		if (nestedLoop[i] == root)
		{
			// add the one behind u
			if (i == 0)
			{
				neighborList.push_back(nestedLoop[i + 1]);
			}
			// add the one before u
			else if (i == nestedLoop.size() - 1 && nestedLoop.size() > 3)
			{
				neighborList.push_back(nestedLoop[i - 1]);
			}
		}
	}


	// replicate root for these new neighbor dudes
	int newId = getNewVertexId();

	Vertex newVertex = Vertex(vertices[root]);
	newVertex.id = newId;
	newVertex.resetNeighbors();
	vertices.push_back(newVertex);
	for (int i = 0; i < neighborList.size(); i++)
	{
		vertices[root].removeNeighbor(neighborList[i]);
		vertices[newId].addNeighbor(neighborList[i]);
		vertices[neighborList[i]].removeNeighbor(root);
		vertices[neighborList[i]].addNeighbor(newId);
	}
}



void Drawing::printPolygons()
{
	for (int i = 0; i < polygons.size(); i++)
	{
		bool flag = false;
		for (int j = 0; j < polygons[i].size(); j++)
		{
			if (polygons[i][j] == 20)
			{
				flag = true;
			}
			cout << polygons[i][j] << " ";
		}
		if (flag == true)
		{
			cout << "<<<<<<<<<<<<<<<<<<";
		}

		cout << endl;
	}
}

vector<Vertex> Drawing::getVerticesByIds(vector<int> vertexIds)
{
	vector<Vertex> temp;

	for (int j = 0; j < vertexIds.size(); j++)
	{
		int id = vertexIds[j];
		Vertex newV = Vertex(vertices[id]);

		newV.resetNeighbors();
		temp.push_back(newV);
	}
	return temp;
}


void Drawing::doEarClipping()
{
	earclippingPolygons.clear();
//	cout << "doEarClipping " << endl;
	//cout << "Printing VerticesGroups" << endl;
	for (int i = 0; i < polygons.size(); i++)
	{
		vector<Vertex> unprocessedPolygonVertices = getVerticesByIds(polygons[i]);

		EarclippingPolygon earclippingPolygon;
		earclippingPolygon.initFromUnprocessedVertices(unprocessedPolygonVertices);

		earclippingPolygons.push_back(earclippingPolygon);
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
	reset();

	mValue content = utl::readJsonFileToMap(filename);

	const mObject& obj = content.get_obj();

//	loadInputRawPoints(obj);
//	loadRawPoints(obj);
	loadVerticesData(obj);
	
	for (int i = 0; i < inputPoints.size(); i++)
	{
		processNewPoint(inputPoints[i]);
	}

//	printPointsAndLines();

//	createVerticesAndEdges();

//	verifyLoadTestDataFunction(vertices, edges, newVertices, newEdges);
}



void Drawing::loadInputRawPoints(const mObject& obj)
{
	const mArray& pointsArray = utl::findValue(obj, "inputPoints").get_array();

	vector<glm::vec2> newPoints;
	for (int i = 0; i < pointsArray.size(); i++)
	{
		const mObject obj = pointsArray[i].get_obj();
		glm::vec2 newP = deserializePoints(obj);
		newPoints.push_back(newP);
	}

	inputPoints = newPoints;

	cout << "points.size " << inputPoints.size() << endl;
	int a = 1;
}


void Drawing::loadRawPoints(const mObject& obj)
{
	const mArray& pointsArray = utl::findValue(obj, "rawPoints").get_array();

	vector<glm::vec2> newPoints;
	for (int i = 0; i < pointsArray.size(); i++)
	{
		const mObject obj = pointsArray[i].get_obj();
		glm::vec2 newP = deserializePoints(obj);
		newPoints.push_back(newP);
	}

	points = newPoints;
	
	cout << "points.size " << points.size() << endl;
	int a = 1;
}

void Drawing::loadVerticesData(const mObject& obj)
{
	const mArray& addr_array = utl::findValue(obj, "vertexData").get_array();

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

Object Drawing::serializePoints(glm::vec2 p)
{
	Object pointObj;

	pointObj.push_back(Pair("x", p.x));
	pointObj.push_back(Pair("y", p.y));

	return pointObj;
}


glm::vec2 Drawing::deserializePoints(const mObject& obj)
{
	glm::vec2 point;
	point.x = utl::findValue(obj, "x").get_real();
	point.y = utl::findValue(obj, "y").get_real();
	return point;
}


void Drawing::saveTestData()
{
	ofstream myfile;
	myfile.open("data.txt");

	Object graphObj;

	Array inputPointsArray;
	for (int i = 0; i < inputPoints.size(); i++)
	{
		Object vObj = serializePoints(inputPoints[i]);
		inputPointsArray.push_back(vObj);
	}

	graphObj.push_back(Pair("inputPoints", inputPointsArray));



	Array pointsArray;
	for (int i = 0; i < points.size(); i++)
	{
		Object vObj = serializePoints(points[i]);
		pointsArray.push_back(vObj);
	}

	graphObj.push_back(Pair("rawPoints", pointsArray));


	Array verticesArray;

	for (int i = 0; i < vertices.size(); i++)
	{
		Object vObj = serializeVertex(vertices[i]);
		verticesArray.push_back(vObj);
	}

	graphObj.push_back(Pair("vertexData", verticesArray));

	write(graphObj, myfile, pretty_print);
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

	bool curBestDirIsCWFromPrevDirFlag = isCWFrom(curBestDir, prevDir);
	int neighborId2 = vCur.neighbors[0];


	// first found adjacent vertex that is not prev is chosen as V_next
	for (int i = 1; i < vCur.neighbors.size(); i++)
	{
		int neighborId = vCur.neighbors[i];
		Vertex vNeighbor = vertices[neighborId];
		glm::vec2 newDir = vNeighbor.pos - vCur.pos;
		
		if (curBestDirIsCWFromPrevDirFlag)
		{
			if (isCWFrom(newDir, prevDir) && isCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCWFromPrevDirFlag = isCWFrom(curBestDir, prevDir);
			}
		}
		else
		{
			if (isCWFrom(newDir, prevDir) || isCWFrom(newDir, curBestDir))
			{
				vNext = vNeighbor;
				curBestDir = newDir;
				curBestDirIsCWFromPrevDirFlag = isCWFrom(curBestDir, prevDir);
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
//	printVerticesAndEdges();
//	cout << " ############### postProcess" << endl;

	if (saveLatest)
	{
		saveTestData();
	}

//	printPolygons();

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

// we need to determine the index by both point0 and point1
// since all of our points are in traversal order
// point0 might have multiple occurences in the points array
// we need it to return the one with the point0->point1 traversal
int Drawing::getPointIndex(glm::vec2 point0, glm::vec2 point1)
{
	for (int i = 0; i < points.size() - 1; i++)
	{
		if(utl::equals(points[i], point0) && utl::equals(points[i+1], point1))
		{
			return i;
		}
	}

	return -1;
}


void Drawing::determinePolygonsInsideOutside()
{
//	cout << "determinePolygonsInsideOutside " << polygons.size() << endl;
	polygonInsideFlags.clear();
	for (int i = 0; i < polygons.size(); i++)
	{
	//	determinePolygonInsideOutside(polygons[i]);
	
		// use the first triangle from your earclippingPolygon
		vector<Vertex> firstTriangle = earclippingPolygons[i].triangles[0];
		glm::vec2 point = findPointInsideOfTriangle(firstTriangle);

		bool isInside = windingNumberPointPolygon(point);
		polygonInsideFlags.push_back(isInside);
	}
}


bool Drawing::windingNumberPointPolygon(glm::vec2 point)
{
	int windingNumber = 0;
	for (int i = 0; i < edges.size(); i++)
	{
		Vertex v0 = vertices[edges[i].id0];
		Vertex v1 = vertices[edges[i].id1];

		// an upward crossing
		if (v0.pos.y <= point.y)
		{
			if (v1.pos.y > point.y)
			{
				if (utl::isPointLeftOfVector(v0.pos, v1.pos, point) > 0)
				{
					windingNumber++;
				}
			}
		}
		else
		{
			if (v1.pos.y <= point.y)
			{
				if (utl::isPointLeftOfVector(v0.pos, v1.pos, point) < 0)
				{
					windingNumber--;
				}
			}
		}
	}

	return windingNumber;
}



glm::vec2 Drawing::findPointInsideOfTriangle(vector<Vertex> triangle)
{
	// just use the centroid
	glm::vec2 centroid = triangle[0].pos + triangle[1].pos + triangle[2].pos;
	centroid = centroid / 3.0f;
	return centroid;
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



#if 0 
void Drawing::nestedSubgraphPostprocess(int root, vector<int> closedWalk, int start, int end)
{
	vector<int> neighborList;

	for (int i = start; i <= end; i++)
	{
		if (closedWalk[i] == root)
		{
			// add the one behind u
			if (i == start)
			{
				neighborList.push_back(closedWalk[i + 1]);
			}
			// add the one before u
			else if (i == end)
			{
				neighborList.push_back(closedWalk[i - 1]);
			}
			// add botht he one behind and before u
			else
			{
				neighborList.push_back(closedWalk[i + 1]);
				neighborList.push_back(closedWalk[i - 1]);
			}
		}
	}

	// replicate root for these new neighbor dudes
	int newId = getNewVertexId();

	Vertex newVertex = Vertex(vertices[root]);
	newVertex.id = newId;
	newVertex.resetNeighbors();
	vertices.push_back(newVertex);
	for (int i = 0; i < neighborList.size(); i++)
	{
		vertices[root].removeNeighbor(neighborList[i]);
		vertices[newId].addNeighbor(neighborList[i]);
		vertices[neighborList[i]].removeNeighbor(root);
		vertices[neighborList[i]].addNeighbor(newId);
	}

}
#endif