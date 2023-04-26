#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#include <thread>
#include <cmath>
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

#include "vec3d.hpp"
#include "Camera.hpp"
#include "Sphere.hpp"
#include "Object.hpp"
#include "utils.hpp"
#include "Ray.hpp"
#include "Display.hpp"


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
