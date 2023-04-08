#pragma once
#include "vec3d.hpp"

class Camera {
public:
	vec3d<double> c_point; // center
	vec3d<double> c_x; // vector coordinates
	vec3d<double> c_y;
	vec3d<double> c_z;
	double cs_dist = 2; // distance from camera to screen
	double view_angle = 50;
	double render_distance = 500;

	Camera(vec3d<double> _c_point, vec3d<double> _c_x, vec3d<double> _c_y, vec3d<double> _c_z)
		: c_point(_c_point),
		c_x(_c_x),
		c_y(_c_y),
		c_z(_c_z)
	{}

	// checks if everything is fine with the camera
	bool is_orthonormal() {
		if ((c_x.lenght() - 1) > 10e-3 || (c_y.lenght() - 1) > 10e-3 || (c_z.lenght() - 1) > 10e-3) return false;
		if (abs(c_x * c_y) > 10e-3 || abs(c_x * c_z) > 10e-3 || abs(c_y * c_z) > 10e-3) return false;
		return true;
	}
};