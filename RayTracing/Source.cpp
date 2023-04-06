#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <cmath>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include "vec3d.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"
#include <thread>

auto convert_to_rad = [](double angle) { return angle * PI / 180.0; };


class Scene {
private:
	//размеры сцены
	const int width;
	const int height;

	//длина экрана в пространстве
	double scr_size;

	// соотношения замера дисплея к его размеру в 3д
	double coeff_width;
	double coeff_height;


	//ширина плитки пола (квадрат)
	double tile_size = 5;

	bool is_floor(vec3d<double>& ray_start_point, vec3d<double>& ray) {
		double coeff = -ray_start_point.z / ray.z;
		if (coeff > 0) {
			if (coeff >= camera.render_distance) return false;
			return is_tile(ray_start_point.x + coeff * ray.x, ray_start_point.y + coeff * ray.y);
		}
		else return false;
	}
	bool is_tile(double x, double y) {
		int x_p = x / tile_size + 1;
		int y_p = y / tile_size + 1;
		if ((x_p + y_p) % 2) return true;
		return false;
	}
public:
	Camera camera;
	std::vector<Sphere> balls;

	//конструктор
	Scene(Camera _camera, const int _width, const int _height)
		: camera(_camera),
		width(_width),
		height(_height)
	{
		scr_size = 2 * camera.cs_dist * tan(convert_to_rad(camera.view_angle) / 2);
		coeff_width = scr_size / (double)width;
		coeff_height = scr_size / (double)height;
	}

	//создаёт картинку которая будет передаваться дисплею
	///(отдать Алексу)
	std::vector<std::vector<olc::Pixel>> make_picture() {
		if (!camera.is_orthonormal()) throw("invalid cameraman");

		std::vector<std::vector<olc::Pixel>> res(width);

		
		for (int i = 0; i < width; ++i)
		{
			res[i].resize(height);

			for (int j = 0; j < height; ++j)
			{
				// преобразование к локальным вещественным координатам
				double scr_loc_x = (double)j * coeff_height - scr_size / 2;
				double scr_loc_y = (double)(width - i) * coeff_width - scr_size / 2;

				// получение глобальной координаты точки на экране
				vec3d<double> scr_dot = camera.c_point + camera.cs_dist * camera.c_z + scr_loc_x * camera.c_x + scr_loc_y * camera.c_y;

				vec3d<double> ray = scr_dot - camera.c_point;

				olc::Pixel color = is_floor(camera.c_point, ray) ? olc::WHITE : olc::BLUE;
				for (auto& sph : balls) {
					auto tmp = -1.0 * ray;
					double intense = sph.is_hitted(camera.c_point, tmp);
					if (intense) {
						double col_intense = intense / ((camera.c_point - sph.center).lenght());
						olc::Pixel col_white(255 - (int)(col_intense * 1000) % 255, 255 - (int)(col_intense * 1000) % 255, 255 - (int)(col_intense * 1000) % 255);
						olc::Pixel col_blue(0, 0, 255 - (int)(col_intense * 1000) % 255);
						auto rotatedVec = sph.reflect_lite(intense, camera.c_point, ray);
						color = is_floor(camera.c_point, rotatedVec) ? col_white : col_blue;
					}
				}
				res[i][j] = color;
			}
		}

		/*
		std::thread th1(&Scene::part_makePicture, this, 0, width / 4,             height, std::ref(res));
		std::thread th2(&Scene::part_makePicture, this, width / 4, width / 2,     height, std::ref(res));
		std::thread th3(&Scene::part_makePicture, this, width / 2, width * 3 / 4, height, std::ref(res));
		std::thread th4(&Scene::part_makePicture, this, width * 3 / 4, width,     height, std::ref(res));
		th1.join();
		th2.join();
		th3.join();
		th4.join();
		*/

		return res;
	}
	
	void part_makePicture(int st_width, int en_width, int height, std::vector<std::vector<olc::Pixel>>& inPut) {
		for (int i = st_width; i < en_width; ++i) {
			inPut[i].resize(height);
			for (int j = 0; j < height; ++j) {

				// преобразование к локальным вещественным координатам
				double scr_loc_x = (double)j * coeff_height - scr_size / 2;
				double scr_loc_y = (double)(width - i) * coeff_width - scr_size / 2;

				// получение глобальной координаты точки на экране
				vec3d<double> scr_dot = camera.c_point + camera.cs_dist * camera.c_z + scr_loc_x * camera.c_x + scr_loc_y * camera.c_y;

				vec3d<double> ray = scr_dot - camera.c_point;

				olc::Pixel color = is_floor(camera.c_point, ray) ? olc::WHITE : olc::BLUE;
				for (auto& sph : balls) {
					auto tmp = -1.0 * ray;
					double intense = sph.is_hitted(camera.c_point, tmp);
					if (intense) {
						double col_intense = intense / ((camera.c_point - sph.center).lenght());
						olc::Pixel col_white(255 - (int)(col_intense * 1000) % 255, 255 - (int)(col_intense * 1000) % 255, 255 - (int)(col_intense * 1000) % 255);
						olc::Pixel col_blue(0, 0, 255 - (int)(col_intense * 1000) % 255);
						auto rotatedVec = sph.reflect_lite(intense, camera.c_point, ray);
						color = is_floor(camera.c_point, rotatedVec) ? col_white : col_blue;
					}
				}
				inPut[i][j] = color;

			}
		}
	}
	
	void add_ball(vec3d<double> _center, double _radius) {
		balls.push_back(Sphere(_center, _radius));
	}
};

class Display : public olc::PixelGameEngine {
public:

	Display(int _width, int _height, Camera camera) :
		width(_width), 
		height(_height), 
		scene(Scene(camera, _width, _height))
	{
		vec3d<double> ball_center(0, -20, 10);
		scene.add_ball(ball_center, 5);
		//scene.add_ball(vec3d<double>(10, -10, 20), 2);
	}
	void print(std::vector<std::vector<olc::Pixel>>& image) {
		int end1 = min(image.size(), width);
		int end2 = min(image[0].size(), height);
		for (int i = 0; i < end1; ++i) {
			for (int j = 0; j < end2; ++j) {
				Draw(j, i, image[i][j]);
			}
		}
	}
	std::pair<int, int> getResolution() {
		return std::make_pair(this->width, this->height);
	}

	bool OnUserCreate() override
	{
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetMouse(0).bHeld)
		{
			std::pair<double, double> newMousePos = { GetMouseX(), GetMouseY() };
			double diffX = newMousePos.first - previousMousePos.first;
			double diffY = newMousePos.second - previousMousePos.second;

			vec3d<double> etalon_y(0, 0, 1);

			scene.camera.c_x = rotateByAxis(scene.camera.c_x, scene.camera.c_x, diffY / 200);
			scene.camera.c_y = rotateByAxis(scene.camera.c_x, scene.camera.c_y, diffY / 200);
			scene.camera.c_z = rotateByAxis(scene.camera.c_x, scene.camera.c_z, diffY / 200);

			scene.camera.c_x = rotateByAxis(etalon_y, scene.camera.c_x, diffX / 200);
			scene.camera.c_y = rotateByAxis(etalon_y, scene.camera.c_y, diffX / 200);
			scene.camera.c_z = rotateByAxis(etalon_y, scene.camera.c_z, diffX / 200);
		}
		previousMousePos = { GetMouseX(), GetMouseY() };


		double step = 1;
		if (GetKey(olc::Key::W).bHeld) {
			scene.camera.c_point = scene.camera.c_point + step * scene.camera.c_z;
		}
		if (GetKey(olc::Key::A).bHeld) {
			scene.camera.c_point = scene.camera.c_point - step * scene.camera.c_x;
		}
		if (GetKey(olc::Key::S).bHeld) {
			scene.camera.c_point = scene.camera.c_point - step * scene.camera.c_z;
		}
		if (GetKey(olc::Key::D).bHeld) {
			scene.camera.c_point = scene.camera.c_point + step * scene.camera.c_x;
		}



		std::vector<std::vector<olc::Pixel>> imageP = scene.make_picture();
		this->print(imageP);
		return true;
	}

private:
	Scene scene;
	const int width = -1;
	const int height = -1;
	clock_t Lasttime = clock();
	bool mousePushed = false;
	std::pair<double, double> previousMousePos = {0, 0};
};


int main()
{
	vec3d<double> a(3, 3, 50);
	vec3d<double> b(1, 0, 0);
	vec3d<double> c(0, 1, 0);
	vec3d<double> d(0, 0, 1);
	Camera camera(a, b, c, d);
	Display demo(720, 720, camera);
	if (demo.Construct(demo.getResolution().second, demo.getResolution().first, 1, 1))
		demo.Start();

	return 0;
}