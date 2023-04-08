#pragma once
#include "vec3d.hpp"
#include <vector>
#include "Triangle.hpp"
#include <fstream>
#include <string>
#include <sstream>
#include "Sphere.hpp"
#include "Ray.hpp"
#include <limits>


class GameObject
{
    using string = std::string;
public:
	GameObject(std::string filename) {
        parser(filename);
        std::cout << filename << ' ' << triagles.size() << ' ' << verteces.size();
        sph = construct_sphere();
	}
    void parser(std::string& filename) {
        double max_x = -10000;
        double min_x = 10000;
        double max_y = -10000;
        double min_y = 10000;
        std::ifstream in(filename, std::ios::in);
        if (!in)
            throw "Cannot open " + filename + "\n";

        string line;
        while (std::getline(in, line))
        {
            string diff_s = "";
            //check v for vertices
            if (line.substr(0, 2) == "v ") {
                std::istringstream v(line.substr(2));
                double x, y, z;
                v >> x; v >> y; v >> z;
                max_x = std::max(max_x, x);
                min_x = std::min(min_x, x);
                max_y = std::max(max_y, y);
                min_y = std::min(min_y, y);

                verteces.push_back(vec3d<double>(x, y, z));
            }
            //check for faces
            else if (line.substr(0, 2) == "f ") {
                line = line.substr(2);
                if (diff_s.size() == 0) {
                    for (int i = 0; i < line.size(); ++i) {
                        if (!std::isdigit(line[i])) {
                            for (int j = i; j < line.size(); ++j) {
                                if (std::isdigit(line[j]))
                                    break;
                                else
                                    diff_s.push_back(line[j]);
                            }
                            break;
                        }
                    }
                }

                int A, B, C; //to store texture index
                int a = get_int(line, diff_s);
                int b = get_int(line, diff_s);
                int c = get_int(line, diff_s);


                a--; b--; c--;
                //A--; B--; C--;
                triagles.push_back(Triangle(verteces[a], verteces[b], verteces[c]));
            }

        }
        std::cout << "max_x " << max_x << '\n';
        std::cout << "min_x " << min_x << '\n';
        std::cout << "max_y " << max_y << '\n';
        std::cout << "min_y " << min_y << '\n';
        std::cout << "verteces " << verteces.size() << '\n';
        std::cout << "triagles " << triagles.size() << '\n';
    }

    double is_hitted(const Ray& inRay) {
        double final_res = 0;
        double last_dist = 10000;
        if (!sph.is_hitted(inRay.stPoint, inRay.vec))
            return 0;
        for (auto& tr : triagles) {
            double res = tr.is_hitted(inRay);
            if (res) {
                //return res;
                if (last_dist > tr.last_dist){
                    final_res = res;
                    last_dist = tr.last_dist;
                    final_res = last_dist;
                }
                    
            }
        }
        return final_res;
    }

private:
	std::vector<Triangle> triagles;
    std::vector<vec3d<double>> verteces;
    Sphere sph;

    Sphere construct_sphere() const {
        vec3d<double> maxX, maxY, maxZ = vec3d<double>(-DBL_MAX, -DBL_MAX, -DBL_MAX);
        vec3d<double> minX, minY, minZ = vec3d<double>(DBL_MAX, DBL_MAX, DBL_MAX);
        for (auto& v : verteces) {
            if (v.x > maxX.x) maxX = v;
            if (v.y > maxY.y) maxY = v;
            if (v.z > maxZ.z) maxZ = v;
            if (v.x < minX.x) minX = v;
            if (v.y < minY.y) minY = v;
            if (v.z < minZ.z) minZ = v;
        }
        std::vector<vec3d<double>> vec = { maxX, maxY, maxZ, minX, minY, minZ };
        std::pair<double, std::pair<vec3d<double>, vec3d<double>>> data; // dist and two points of it
        // find two points with max dist
        for (auto& v1 : vec) {
            for (auto& v2 : vec) {
                double d = dist(v1, v2);
                if (d > data.first) {
                    data.first = d;
                    data.second = { v1, v2 };
                }
            }
        }
        vec3d<double> center((data.second.first + data.second.second) * 0.5);
        double radius = dist(center, data.second.first);
        return Sphere(center, radius);
    }

    int get_int(string& line, string& diff_symbol) {
        string num_s;
        for (int i = 0; i < line.size(); ++i) {
            if (std::isdigit(line[i])) {
                num_s.push_back(line[i]);
            }
            else {
                if (diff_symbol == line.substr(i, diff_symbol.size())) {
                    int a = std::atof(num_s.c_str());
                    num_s.clear();
                    line = line.substr(i + diff_symbol.size());
                    return a;
                }
                if (num_s.size() == 0) {
                    throw "strange";
                }
                throw "strange";
            }
        }
        if (num_s.size() == 0) {
            throw "strange";
        }
        else {
            int a = std::atof(num_s.c_str());
            num_s.clear();
            line.clear();
            return a;
        }
    }
};
