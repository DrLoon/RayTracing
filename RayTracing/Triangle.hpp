#pragma once
#include <limits>

#include "vec3d.hpp"
#include "Ray.hpp"
#include "utils.hpp"


class Plane {
public:
	Plane(const vec3d<double>& _Point1, const vec3d<double>& _Point2, const vec3d<double>& _Point3)
		: Point1(_Point1), Point2(_Point2), Point3(_Point3) 
	{
		A = (Point2.y - Point1.y) * (Point3.z - Point1.z) - (Point2.z - Point1.z) * (Point3.y - Point1.y);
		B = (Point3.x - Point1.x) * (Point2.z - Point1.z) - (Point3.z - Point1.z) * (Point2.x - Point1.x);
		C = (Point2.x - Point1.x) * (Point3.y - Point1.y) - (Point2.y - Point1.y) * (Point3.x - Point1.x);
		double D1 = -Point1.x * (Point2.y - Point1.y) * (Point3.z - Point1.z) + (Point2.x - Point1.x) * (Point3.y - Point1.y)
			* (-Point1.z) + (Point3.x - Point1.x) * (Point2.z - Point1.z) * (-Point1.y);
		double D2 = -Point1.z * (Point2.y - Point1.y) * (Point3.x - Point1.x) + (Point2.z - Point1.z) * (Point3.y - Point1.y)
			* (-Point1.x) + (Point3.z - Point1.z) * (Point2.x - Point1.x) * (-Point1.y);
		D = D1 - D2;
	};

	bool is_belongs(const vec3d<double>& point) const {
		// TODO unused function
		double _1 = (point.x - Point1.x) * (Point2.y - Point1.y) * (Point3.z - Point1.z);
		double _2 = (Point2.x - Point1.x) * (Point3.y - Point1.y) * (point.z - Point1.z);
		double _3 = (Point3.x - Point1.x) * (point.y - Point1.y) * (Point2.z - Point1.z);
		double _4 = (point.z - Point1.z) * (Point2.y - Point1.y) * (Point3.x - Point1.x);
		double _5 = (Point2.z - Point1.z) * (Point3.y - Point1.y) * (point.x - Point1.x);
		double _6 = (Point3.z - Point1.z) * (point.y - Point1.y) * (Point2.x - Point1.x);

		double res = _1 + _2 + _3 - _4 - _5 - _6;

		if (abs(res) < 10e-5)
			return true;
		return false;
	}

	bool is_belongs_fast(const vec3d<double>& point) const {
		// TODO unused function
		double res = A * point.x + B * point.y + C * point.z + D;

		if (abs(res) < 10e-15)
			return true;
		return false;
	}

	std::pair<vec3d<double>, double> cross_with_line(const Ray& ray) const {
		// TODO unused function
		vec3d<double> n = get_normal();
		double k = ((ray.stPoint * n + D) / (ray.vec * n));
		vec3d<double> s = ray.vec * k;
		vec3d<double> res = ray.stPoint - s;
		return std::make_pair(res, k);
	}

	vec3d<double> cross_with_line(const Ray& ray, const double k) const {
		return ray.stPoint - ray.vec * k;
	}

	double is_meaning(const Ray& ray) const {
		// TODO rename and describe
		auto n = get_normal();
		//return n * ray.vec;
		return ((ray.stPoint * n + D) / (ray.vec * n));
	}

	vec3d<double> get_normal() const {
		return vec3d<double>(A, B, C);
	}
	
private:
	vec3d<double> Point1;
	vec3d<double> Point2;
	vec3d<double> Point3;

	double A, B, C, D;
};


class Triangle {
public:
	double intense = 1; // intensity in special cases is lost after reflections
	double last_dist = DBL_MAX;

	Triangle(vec3d<double> _Point1, vec3d<double> _Point2, vec3d<double> _Point3) : Point1(_Point1), Point2(_Point2), Point3(_Point3),
		plane(Point1, Point2, Point3) {
		vec3d<double> light(1, -1, -2);  // TODO bad place for light point
		color = light * plane.get_normal();
	}

	double is_hitted(const Ray& inRay) {
		double a = plane.is_meaning(inRay);
		if (plane.get_normal() * inRay.vec <= 0) {
			return 0;
		}
		vec3d<double> cross_point = plane.cross_with_line(inRay, a);
		last_dist = abs((cross_point - inRay.stPoint) * vec3d<double>(1, 1, 1));
		return color * isInside_flat(cross_point);
	}

	bool isInside_flat(const vec3d<double>& pt) const {
		if ((Point1.x == Point2.x && Point2.x == Point3.x) || ((Point1.y == Point2.y && Point2.y == Point3.y)))
			return isInsideTriangle(pt);
		auto a = (Point1.x - pt.x) * (Point2.y - Point1.y) - (Point2.x - Point1.x) * (Point1.y - pt.y);
		auto b = (Point2.x - pt.x) * (Point3.y - Point2.y) - (Point3.x - Point2.x) * (Point2.y - pt.y);
		auto c = (Point3.x - pt.x) * (Point1.y - Point3.y) - (Point1.x - Point3.x) * (Point3.y - pt.y);
		if (sign(a) == sign(b) && sign(b) == sign(c))
			return true;
		return false;
	}

	bool isInsideTriangle(const vec3d<double>& pt) const {
		vec3d<double> n = plane.get_normal();
		double TRIANGLE_EPSILON = 0;
		if (cross_product(Point2 - Point1, pt - Point1) * n < TRIANGLE_EPSILON)
			return false;
		if (cross_product(pt - Point1, Point3 - Point1) * n < TRIANGLE_EPSILON)
			return false;
		if (cross_product(Point2 - pt, Point3 - pt) * n < TRIANGLE_EPSILON)
			return false;
		return true;
	}

	vec3d<double> get_normal() const {
		return cross_product((Point2 - Point1), (Point3 - Point1));
	}

	std::tuple<vec3d<double>, vec3d<double>, vec3d<double>> get_points() const {
		return { Point1 , Point2 , Point3 };
	}

private:
	vec3d<double> Point1;
	vec3d<double> Point2;
	vec3d<double> Point3;

	double color = 1;
	
	Plane plane;
};
