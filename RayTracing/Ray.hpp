#pragma once
#include "vec3d.hpp"


class Ray {
public:
	vec3d<double> stPoint; // starting point
	vec3d<double> vec; // the vector itself
	int countRecflections = 0;
	double intense = 1; // intensity in special cases is lost after reflections

	Ray(const vec3d<double>& _stPoint, const vec3d<double>& _vec) : stPoint(_stPoint), vec(_vec) {}
private:

};
