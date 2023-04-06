#pragma once
#include "vec3d.hpp"

class Ray {
public:
	vec3d<double> stPoint; // начальная точка
	vec3d<double> vec; // сам вектор
	int countRecflections = 0;
	double intense = 1; // интенсивность в особых случаях теряется после отражений
	Ray(vec3d<double> _stPoint, vec3d<double> _vec) : stPoint(_stPoint), vec(_vec) {}
private:

};