#ifndef BEZIER_POINTS_H_
#define BEZIER_POINTS_H_

#include <iostream>
#include <cstdlib>

#include "pipeline.h"
#include "Model.h"

using namespace std;

class BezierPoints : public Model
{
public:
	// Constructor
	BezierPoints();
	~BezierPoints();

	glm::vec3 calculateBezierPoint(float t, glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
};


#endif