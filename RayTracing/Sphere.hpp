#pragma once
#include "vec3d.hpp"

class Sphere {
public:
	vec3d<double> center;
	double radius;
	Sphere(vec3d<double> _center, double _radius) :
		center(_center),
		radius(_radius)
	{}
	double is_hitted(const vec3d<double>& ray_start_point, const vec3d<double>& ray) {
		vec3d<double> to_center = ray_start_point - center;

		double a = ray.x * ray.x + ray.y * ray.y + ray.z * ray.z;
		double b = -2 * (to_center.x * ray.x + to_center.y * ray.y + to_center.z * ray.z);
		double c = to_center.x * to_center.x + to_center.y * to_center.y + to_center.z * to_center.z - radius * radius;

		//дискриминант 
		double Discriminant = b * b - 4 * a * c;
		if (Discriminant < 0) return 0;
		double coeff1 = (-b + sqrt(Discriminant)) / (2 * a);
		double coeff2 = (-b - sqrt(Discriminant)) / (2 * a);
		if (coeff1 <= 0 || coeff2 <= 0) return 0;
		return min(coeff1, coeff2);
	}
	vec3d<double> reflect_hit(double coeff, const vec3d<double>& ray_start_point, vec3d<double>& ray) {
		//vec3d<double> hit_point = ray_start_point + ray * coeff;
		vec3d<double> centerToHitPoint = ray_start_point + ray * coeff - this->center;

		auto res = cross_product(centerToHitPoint, ray);
		double angleB_CR =  acos(centerToHitPoint * ray / (centerToHitPoint.lenght() * ray.lenght()));

		return -1.0 * rotateByAxis(res, ray,  (2 * PI - 2 * angleB_CR));
	}
	vec3d<double> reflect_lite(double coeff, const vec3d<double>& ray_start_point, vec3d<double>& ray) {
		vec3d<double> centerToHitPoint = ray_start_point + ray * coeff - this->center;
		centerToHitPoint.normalize();
		return ray - 2 * (ray * centerToHitPoint) * centerToHitPoint;
	}
};