#pragma once
#include <thread>
#include <cmath>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <mutex>

#include "vec3d.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"
#include "Object.hpp"
#include "utils.hpp"
#include "Ray.hpp"

class Scene {
private:
	// scene sizes
	const int width;
	const int height;

	// the length of the screen in space
	double scr_size;

	// the ratio of the measue of the display to its size in 3d
	double coeff_width;
	double coeff_height;

	// floor tile width (square)
	double tile_size = 5;

	const int threads_number = 4;
	std::vector<std::thread> thread_s;
	std::vector<int> is_done;
	std::mutex mut_dist;
	int done_number = 0;
	std::mutex cv_mut;
	std::condition_variable cv;
	bool life_time_finished = false;

	std::vector<std::vector<olc::Pixel>> res_image;

	bool is_floor(const Ray& ray) const {
		double coeff = -ray.stPoint.z / ray.vec.z;
		if (coeff > 0) {
			if (coeff >= camera.render_distance) return false;
			return is_tile(ray.stPoint.x + coeff * ray.vec.x, ray.stPoint.y + coeff * ray.vec.y);
		}
		else return false;
	}

	bool is_under_cow(const Ray& ray) const {
		// TODO delete this
		double coeff = -ray.stPoint.z / ray.vec.z;
		if (coeff > 0) {
			if (coeff >= camera.render_distance) return false;
			return ray.stPoint.x + coeff * ray.vec.x > -4.44583
				&& ray.stPoint.x + coeff * ray.vec.x < 5.99809
				&& ray.stPoint.y + coeff * ray.vec.y > -3.63704
				&& ray.stPoint.y + coeff * ray.vec.y < 2.75972;
		}
		else return false;
	}

	bool is_tile(const double x, const double y) const {
		int x_p = x / tile_size + 1;
		int y_p = y / tile_size + 1;
		if ((x_p + y_p) % 2) return true;
		return false;
	}

public:
	Camera camera;
	std::vector<Sphere> balls;
	std::vector<GameObject> g_objects;

	Scene(Camera _camera, const int _width, const int _height)
		: camera(_camera),
		width(_width),
		height(_height)
	{
		// TODO fix height width problem
		scr_size = 2 * camera.cs_dist * tan(convert_to_rad(camera.view_angle) / 2);
		coeff_width = scr_size / (double)width;
		coeff_height = scr_size / (double)height;

		res_image.resize(width);
		for (auto& i : res_image)
			i.resize(height);

		is_done.resize(width, 1);

		thread_s.resize(threads_number);
		for (int i = 0; i < threads_number; ++i)
			thread_s[i] = std::thread(&Scene::distributed_exe_f, this, std::ref(res_image), std::ref(is_done));
	}

	//destructor
	~Scene() {
		mut_dist.lock();
		life_time_finished = true;
		mut_dist.unlock();

		for (auto& i : thread_s)
			i.join();
	}

	void distributed_exe_f(std::vector<std::vector<olc::Pixel>>& res_image, std::vector<int>& isDone) {
		for (int i = 0; i < res_image.size(); ++i) {
			mut_dist.lock();
			if (life_time_finished) { mut_dist.unlock(); break; }
			if (!isDone[i]) {
				isDone[i] = 1;
				mut_dist.unlock();
				for (int j = 0; j < res_image.size(); ++j)
					res_image[i][j] = calculate_ray(i, j);
				mut_dist.lock();
				isDone[i] = 2;
				std::lock_guard<std::mutex> lk(cv_mut);
				++done_number;
			}
			mut_dist.unlock();
			cv.notify_one(); //mayby switch to all in future
			if (i == res_image.size() - 1) i = -1;
		}
	}

	olc::Pixel calculate_ray(const int i, const int j) {
		double scr_loc_x = (double)j * coeff_height - scr_size / 2;
		double scr_loc_y = (double)(width - (double)i) * coeff_width - scr_size / 2;

		vec3d<double> scr_dot = camera.c_point + camera.cs_dist * camera.c_z + scr_loc_x * camera.c_x + scr_loc_y * camera.c_y;

		vec3d<double> ray = scr_dot - camera.c_point;

		Ray RAY(camera.c_point, ray);

		olc::Pixel color = is_floor(RAY) ? olc::WHITE : olc::BLUE;
		color = is_under_cow(RAY) ? olc::YELLOW : color;

		RAY.vec = RAY.vec * -1.0; // TODO bug!!!

		for (auto& sph : balls) {
			double intense = sph.is_hitted(RAY);
			if (intense) {
				double col_intense = intense / ((camera.c_point - sph.center).lenght());
				olc::Pixel col_white(255 - (int)(col_intense * 1000) % 255, 255 - (int)(col_intense * 1000) % 255, 255 - (int)(col_intense * 1000) % 255);
				olc::Pixel col_blue(0, 0, 255 - (int)(col_intense * 1000) % 255);
				auto rotatedVec = sph.reflect_lite(intense, camera.c_point, ray);
				color = is_floor(Ray(camera.c_point, rotatedVec)) ? col_white : col_blue;
			}
		}

		for (auto& g_object : g_objects) {
			double intense = g_object.is_hitted(RAY);
			if (intense) {
				intense /= 3;  // TODO fix
				olc::Pixel col_green(0, 100 + (int)(abs(intense)) % 255, 0);
				color = col_green;
			}
		}

		return color;
	}

	std::vector<std::vector<olc::Pixel>> make_picture() {
		if (!camera.is_orthonormal()) throw("invalid cameraman");
		mut_dist.lock();
		for (int& i : is_done)
			i = 0;
		done_number = 0;
		mut_dist.unlock();

		std::unique_lock<std::mutex> lk(cv_mut);
		cv.wait(lk, [this] { return done_number == width; });

		return res_image;
	}

	void add_ball(vec3d<double> _center, double _radius) {
		balls.push_back(Sphere(_center, _radius));
	}
	void add_game_object(const std::string filename) {
		g_objects.push_back(GameObject(filename));
	}
};
