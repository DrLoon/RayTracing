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
#include "Object.hpp"
#include <armadillo>

auto convert_to_rad = [](double angle) { return angle * PI / 180.0; };


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

	//������ �������
	int threads_number = 10;
	std::vector<std::thread> thread_s;
	std::vector<int> is_done;
	std::mutex mut_dist;
	int done_number = 0;
	std::mutex cv_mut;
	std::condition_variable cv;
	bool life_time_finished = false;

	//����� 
	std::vector<std::vector<olc::Pixel>> res_image;
	

	bool is_floor(vec3d<double>& ray_start_point, vec3d<double>& ray) {
		double coeff = -ray_start_point.z / ray.z;
		if (coeff > 0) {
			if (coeff >= camera.render_distance) return false;
			return is_tile(ray_start_point.x + coeff * ray.x, ray_start_point.y + coeff * ray.y);
		}
		else return false;
	}
	bool is_under_cow(vec3d<double>& ray_start_point, vec3d<double>& ray) {
		double coeff = -ray_start_point.z / ray.z;
		if (coeff > 0) {
			if (coeff >= camera.render_distance) return false;
			return ray_start_point.x + coeff * ray.x > -4.44583
				&& ray_start_point.x + coeff * ray.x < 5.99809
				&& ray_start_point.y + coeff * ray.y > -3.63704
				&& ray_start_point.y + coeff * ray.y < 2.75972;
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
	std::vector<GameObject> g_objects;

	Scene(Camera _camera, const int _width, const int _height)
		: camera(_camera),
		width(_width),
		height(_height)
	{
		scr_size = 2 * camera.cs_dist * tan(convert_to_rad(camera.view_angle) / 2);
		coeff_width = scr_size / (double)width;
		coeff_height = scr_size / (double)height;

		//������ ������ ��������
		res_image.resize(width);
		for (auto& i : res_image) i.resize(height); 
		is_done.resize(width, 1);
		//������ ������ 
		thread_s.resize(threads_number);
		for (int i = 0; i < threads_number; ++i)
			thread_s[i] = std::thread(&Scene::distributed_exe_f, this, std::ref(res_image), std::ref(is_done));
	}

	//destructor
	~Scene() {
		mut_dist.lock();
		life_time_finished = true;
		mut_dist.unlock();

		for (auto& i : thread_s) i.join();
	}

	//��� ������� 
	void distributed_exe_f(std::vector<std::vector<olc::Pixel>>& res_image, std::vector<int>& isDone) {
		for (int i = 0; i < res_image.size(); ++i) {
			mut_dist.lock();
			if (life_time_finished) { mut_dist.unlock(); break; }
			if (!isDone[i]) {
				isDone[i] = 1;
				mut_dist.unlock();
				std::vector<olc::Pixel> colomn(height);
				for(int j = 0; j < colomn.size(); ++j)	
					colomn[j] = culcelate_ray(i, j);
				mut_dist.lock();
				res_image[i] = std::move(colomn);
				isDone[i] = 2;
				std::lock_guard<std::mutex> lk(cv_mut);
				++done_number;
			}
			mut_dist.unlock();
			cv.notify_one(); //mayby switch to all in future
			if (i == res_image.size() - 1) i = -1;
		}
	}

	//����������� ����
	olc::Pixel culcelate_ray(int i, int j) {
		// �������������� � ��������� ������������ �����������
		double scr_loc_x = (double)j * coeff_height - scr_size / 2;
		double scr_loc_y = (double)(width - i) * coeff_width - scr_size / 2;

		// ��������� ���������� ���������� ����� �� ������
		vec3d<double> scr_dot = camera.c_point + camera.cs_dist * camera.c_z + scr_loc_x * camera.c_x + scr_loc_y * camera.c_y;

		vec3d<double> ray = scr_dot - camera.c_point;

		olc::Pixel color = is_floor(camera.c_point, ray) ? olc::WHITE : olc::BLUE;
		color = is_under_cow(camera.c_point, ray) ? olc::YELLOW : color;
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
		for (auto& g_object : g_objects) {
			auto tmp = -1.0 * ray;
			Ray ray(camera.c_point, tmp);
			double intense = g_object.is_hitted(ray);
			if (intense) {
				intense /= 3;
				olc::Pixel col_green(0, 100 + (int)(abs(intense)) % 255, 0);
				color = col_green;
			}
		}

		return color;
	}

	std::vector<std::vector<olc::Pixel>> make_picture() {
		if (!camera.is_orthonormal()) throw("invalid cameraman");
		mut_dist.lock();
		for (int& i : is_done) i = 0;
		done_number = 0;
		mut_dist.unlock();

		std::unique_lock<std::mutex> lk(cv_mut);
		cv.wait(lk, [this] {return done_number == width; });

		return res_image;
	}

	
	void add_ball(vec3d<double> _center, double _radius) {
		balls.push_back(Sphere(_center, _radius));
	}
	void add_game_object(std::string filename) {
		g_objects.push_back(GameObject(filename));
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
		//scene.add_alotof_balls("res_cow.txt");
		//scene.add_ball(ball_center, 5);
		//scene.add_game_object("models/cube.obj");
		//scene.add_game_object("models/cow-nonormals.obj");

		scene.add_game_object("models/chess_simplest/knight.obj");
		//scene.add_game_object("models/chess_simplest/pawn.obj");
		//scene.add_game_object("models/chess_simplest/bishop.obj");
		//scene.add_game_object("models/chess_simplest/king.obj");
		//scene.add_game_object("models/chess_simplest/queen.obj");
		//scene.add_game_object("models/chess_simplest/rook.obj");
		
		//for(int i = 0; i < 10000; ++i) scene.add_ball(vec3d<double>(10 * i, -10, 20), 2);

		//scene.create_dataset("cube_data.txt");
	}
	void print(std::vector<std::vector<olc::Pixel>>& image) {
		int end1 = std::min((int)image.size(), width);
		int end2 = std::min((int)image[0].size(), height);
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


		double step = 1.0 * fElapsedTime / 0.02;
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
	vec3d<double> a(0, 0, 1);
	vec3d<double> b(1, 0, 0);
	vec3d<double> c(0, 1, 0);
	vec3d<double> d(0, 0, 1);
	Camera camera(a, b, c, d);
	Display demo(400, 400, camera);
	if (demo.Construct(demo.getResolution().second, demo.getResolution().first, 2, 2, false))
		demo.Start();

	return 0;
}