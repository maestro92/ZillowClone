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

	cout << endl << endl;
	triangulate();
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

		calculateInteriorAngle(v);
		
		/*
		if (v.angle > 180)
		{
			reflexVertices.push_back(v.id);
		}
		else if (v.angle < 180)
		{
			convexVertices.push_back(v.id);
		}
		else
		{
			cout << "Something is off" << endl;
		}
		*/
	}	
}


void EarclippingPolygon::printConvexVertices()
{
	cout << "Printing ConvexVertices" << endl;
	for (int i = 0; i < convexVertices.size(); i++)
	{
		cout << "(" << debugMap[convexVertices[i]] << ") ";
	}

	cout << endl;

	for (int i = 0; i < convexVertices.size(); i++)
	{
		cout << convexVertices[i] << " ";
	}


	cout << endl;

}



void EarclippingPolygon::printReflexVertices()
{
	cout << "Print ReflexVertices " << endl;
	for (int i = 0; i < reflexVertices.size(); i++)
	{
		cout << "(" << debugMap[reflexVertices[i]] << ") ";
	}
	cout << endl;

	for (int i = 0; i < reflexVertices.size(); i++)
	{
		cout << reflexVertices[i] << " ";
	}
	cout << endl;
}


void EarclippingPolygon::printEars()
{
	cout << "printing ears " << endl;
	for (int i = 0; i < ears.size(); i++)
	{
		cout << "(" << debugMap[ears[i]] << ") ";
	}

	cout << endl;
	for (int i = 0; i < ears.size(); i++)
	{
		cout << ears[i] << " ";
	}
	cout << endl;

}


bool EarclippingPolygon::isRemoved(int id)
{
	for (int i = 0; i < removed.size(); i++)
	{
		if (removed[i] == id)
		{
			return true;
		}
	}
	return false;
}

void EarclippingPolygon::findConvexReflexAndEars()
{
	reflexVertices.clear();
	convexVertices.clear();
	ears.clear();
	for (int i = 0; i < vertices.size(); i++)
	{
		Vertex v = vertices[i];

		if (!isRemoved(v.id))
		{
			if (v.angle > 180)
			{
				reflexVertices.push_back(v.id);
			}
			else if (v.angle < 180)
			{
				convexVertices.push_back(v.id);
			}
			else
			{
				cout << "Something is off" << endl;
			}
		}
	}

	// is ear depends on the reflex and convex list, so we need to do them separately
	for (int i = 0; i < convexVertices.size(); i++)
	{
		Vertex v = vertices[convexVertices[i]];
		if (!isRemoved(v.id))
		{
			if (isEar(v.id))
			{
				ears.push_back(v.id);
			}
		}
	}

}


void EarclippingPolygon::triangulate()
{
	saveOriginalVertices();
	// verifying initial lists for reflex vertices, and ears
	findConvexReflexAndEars();

	printConvexVertices();
	printReflexVertices();
	printEars();

	//	print();

	int i = 0;
	Vertex v0, v1, v2;

	if (vertices.size() > 3)
	{
		while (removed.size() != vertices.size() - 3)
		{
			cout << endl << endl << ">>>>>>>>>>> iteration " << i << endl;
			int id = ears[0];
			Vertex& curr = vertices[id];
			ears.erase(ears.begin());

			cout << endl << endl << "	removing " << id << endl;

			removed.push_back(curr.id);

			// remove neighbors

			Vertex& prev = vertices[curr.neighbors[0]];
			Vertex& next = vertices[curr.neighbors[1]];

			curr.resetNeighbors();
			prev.neighbors[1] = next.id;
			next.neighbors[0] = prev.id;

			addTriangleFromEarclipping(vertices[prev.id], vertices[curr.id], vertices[next.id]);


			calculateInteriorAngle(prev);
			calculateInteriorAngle(next);
			findConvexReflexAndEars();

			i++;

			printConvexVertices();
			printReflexVertices();
			printEars();
		}
	}

	// handle the last triangle
	int id = ears[0];
	Vertex& curr = vertices[id];
	Vertex& prev = vertices[curr.neighbors[0]];
	Vertex& next = vertices[curr.neighbors[1]];
	addTriangleFromEarclipping(vertices[prev.id], vertices[curr.id], vertices[next.id]);
}


void EarclippingPolygon::addTriangleFromEarclipping(Vertex v0, Vertex v1, Vertex v2)
{
	vector<Vertex> triangle;

	triangle.push_back(v0);
	triangle.push_back(v1);
	triangle.push_back(v2);

	cout << "adding triangle " << v0.id << " " << v1.id << " " << v2.id << endl;

	triangles.push_back(triangle);
}


// https://stackoverflow.com/questions/28821329/interior-angles-of-irregular-polygon-with-angles-180

void EarclippingPolygon::calculateInteriorAngle(Vertex& v)
{
	// orders matter
	if (v.neighbors.size() == 2)
	{
		int prevId = v.neighbors[0];
		int nextId = v.neighbors[1];
		Vertex prev = vertices[prevId];
		Vertex next = vertices[nextId];

		glm::vec2 dPrev = glm::normalize(v.pos - prev.pos);
		glm::vec2 dNext = glm::normalize(next.pos - v.pos);
		
		// the atan2 function return arctan y/x in the interval [-pi, +pi] radians
		double theta0 = atan2(dPrev.y, dPrev.x) * 180 / M_PI;
		double theta1 = atan2(dNext.y, dNext.x) * 180 / M_PI;

		// handle angle wrap around

		/*
			the idea is that
			theta0 is in -pi to pi
			theta1 is in -pi to pi

			when you calculate theta0 - theta1
			it can be -2pi to 2pi

			first we add 180, so that it is in
			-pi to 3pi

			then we add 360 to make -pi disappear.
			cuz if you think about it, when you have a angle in -pi to pi

			the mapping is 
					angle = angle + 360

					if angle > 360
						angle -= 360

			this works cuz
			if our angle is negative
				adding 360 converts your angle (if it is negative) to positive.
			
			if our angle is possitive
				adding 360 is still the samething. The subsequent if statement clamps it in the [0, 360] range

			so same thing here, once our angle is in -pi to 3pi range
			we do the samething
			*/
		float diff = theta0 - theta1;
		float angle = (180 + diff + 360);
		while (angle > 360)
		{
			angle -= 360;
		}
		v.angle = angle;

		/*
		if (angle > 180)
		{
			reflexVertices.push_back(v.id);
		}
		else if (angle < 180)
		{
			convexVertices.push_back(v.id);
		}
		else
		{
			cout << "Something is off" << endl;
		}
		*/

	}
	else
	{
		cout << "Something is wrong, num of neighbors is incorrect " << endl;
	}

}


void EarclippingPolygon::saveOriginalVertices()
{
	backupVertices = vertices;
}

vector<int> EarclippingPolygon::getTriangleFormedByAdjacentVerticies(int id)
{
	vector<int> triangle;
	Vertex v = vertices[id];
	triangle.push_back(v.neighbors[0]);
	triangle.push_back(v.id);
	triangle.push_back(v.neighbors[1]);

	return triangle;
}


bool EarclippingPolygon::isEar(int id)
{
	vector<int> triangle = getTriangleFormedByAdjacentVerticies(id);

	Vertex v0 = vertices[triangle[0]];
	Vertex v1 = vertices[triangle[1]];
	Vertex v2 = vertices[triangle[2]];

	return !checkIfOtherVerticesAreInTriangle(v0, v1, v2);
}

void EarclippingPolygon::findEars()
{
	Vertex v0, v1, v2;

	// we check if convex vertices whether they are ears
	for (int j = 0; j < convexVertices.size(); j++)
	{
		int id = convexVertices[j];
		if (isEar(id))
		{
			ears.push_back(v1.id);
		}
	}
}


bool EarclippingPolygon::checkIfOtherVerticesAreInTriangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) const
{
	// the paper says it's sufficient to just test against the reflex vertices.. soo 
	for (int i = 0; i < reflexVertices.size(); i++)
	{
		Vertex v = vertices[ reflexVertices[i] ];

		if (v == v0 || v == v1 || v == v2)
		{
			continue;
		}
		else
		{
			glm::vec2 test = v.pos;
			bool isInside = utl::isPointInTriangle(test, v0.pos, v1.pos, v2.pos);
			if (isInside)
			{
				return true;
			}
		}
	}
	return false;
}


void EarclippingPolygon::print()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		cout << "originally " << debugMap[vertices[i].id] << endl;
		vertices[i].print();
	}



	for (int i = 0; i < triangles.size(); i++)
	{
		vector<Vertex> triangle = triangles[i];
		Vertex v0 = triangle[0];
		Vertex v1 = triangle[1];
		Vertex v2 = triangle[2];

		cout << "ori: " << debugMap[v0.id] << " " << debugMap[v1.id] << " " << debugMap[v2.id] << endl;
		cout << "new: " << v0.id << " " << v1.id << " " << v2.id << endl << endl;;
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



