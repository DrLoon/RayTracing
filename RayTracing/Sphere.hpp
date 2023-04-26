#pragma once
#include "vec3d.hpp"
#include "Ray.hpp"


class Sphere {
public:
	vec3d<double> center;
	double radius;

	Sphere(vec3d<double> _center, double _radius) :
		center(_center),
		radius(_radius)
	{}

	Sphere() : radius(1) {}

	double is_hitted(const Ray& ray) const {
		vec3d<double> to_center = ray.stPoint - center;

		double a = ray.vec * ray.vec;
		double b = -2 * (to_center * ray.vec);
		double c = to_center * to_center - radius * radius;

		double Discriminant = b * b - 4 * a * c;
		if (Discriminant < 0) return 0;

		double coeff1 = (-b + sqrt(Discriminant)) / (2 * a);
		double coeff2 = (-b - sqrt(Discriminant)) / (2 * a);
		if (coeff1 <= 0 || coeff2 <= 0) return 0;
		return std::min(coeff1, coeff2);
	}

	bool is_hitted_lite(const Ray& ray) const {
		// TODO unused function, slower than is_hitted
		vec3d<double> to_center = ray.stPoint - center;
		if (to_center * ray.vec <= 0) return false;
		double d = cross_product(to_center, ray.vec).lenght() / ray.vec.lenght();
		return d <= radius;
	}

	vec3d<double> reflect_hit(const double coeff, const vec3d<double>& ray_start_point, vec3d<double>& ray) const {
		// TODO unused function
		//vec3d<double> hit_point = ray_start_point + ray * coeff;
		vec3d<double> centerToHitPoint = ray_start_point + ray * coeff - this->center;

		auto res = cross_product(centerToHitPoint, ray);
		double angleB_CR =  acos(centerToHitPoint * ray / (centerToHitPoint.lenght() * ray.lenght()));

		return -1.0 * rotateByAxis(res, ray,  (2 * PI - 2 * angleB_CR));
	}

	vec3d<double> reflect_lite(const double coeff, const vec3d<double>& ray_start_point, vec3d<double>& ray) const {
		vec3d<double> centerToHitPoint = ray_start_point + ray * coeff - this->center;
		centerToHitPoint.normalize();
		return ray - 2 * (ray * centerToHitPoint) * centerToHitPoint;
	}
};
