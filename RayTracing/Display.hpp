#pragma once
#include <iostream>
#include <vector>
#include <string>

#include "vec3d.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"
#include "Object.hpp"
#include "Ray.hpp"
#include "Scene.hpp"
#include "olcPixelGameEngine.h"

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
	}

	void print(const std::vector<std::vector<olc::Pixel>>& image) {
		int end1 = std::min((int)image.size(), width);
		int end2 = std::min((int)image[0].size(), height);
		for (int i = 0; i < end1; ++i) {
			for (int j = 0; j < end2; ++j) {
				Draw(j, i, image[i][j]);
			}
		}
	}

	std::pair<int, int> getResolution() const {
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
	std::pair<double, double> previousMousePos = { 0, 0 };
};