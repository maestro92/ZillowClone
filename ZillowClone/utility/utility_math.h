#ifndef UTILITY_MATH_H_
#define UTILITY_MATH_H_
#include <sstream> //You'll need to include this header to use 'std::stringstream'.
#include <iostream>
#include <iomanip>
#include "define.h"
#include <vector>

using namespace std;

namespace utl
{

//	static const float MATH_EPSILON;
	/// utl_Math.cpp
	string intToStr(int value);
	string floatToStr(float value);
	string vec2ToStr(glm::vec2 value);
	int randInt(int min = 0, int max = 100);
	float randFloat(float min = 0, float max = 1);
	template<class T>
	inline T sqr(const T &x)
	{
		return x * x;
	}
	
	bool sameSign(float a, float b);

	int isPointLeftOfVector(glm::vec2 v0, glm::vec2 v1, glm::vec2 poing);

	float crossProduct2D(glm::vec2 a, glm::vec2 b);

	bool chance(float percent);

	bool equals(glm::vec2 a, glm::vec2 b);
	float barycentricInterpolation(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec2 coord);

	glm::vec3 interpolateEntityPosition(glm::vec3 pos0, glm::vec3 pos1, float interpFactor);
	glm::vec3 interpolateEntityAngles(glm::vec3 pos0, glm::vec3 pos1, float interpFactor);
	float interpolateAngle(float f0, float f1, float interpFactor);

	bool isPointInTriangle(glm::vec2 point, glm::vec2 v0, glm::vec2 v1, glm::vec2 v2);
	bool isPointInTriangle(glm::vec3 point, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2);


	float sqDistBetweenPointAndLineSetment(glm::vec2 l0, glm::vec2 l1, glm::vec2 p);
	float sqDistBetweenPointAndLineSetment(glm::vec3 l0, glm::vec3 l1, glm::vec3 p);

}
#endif