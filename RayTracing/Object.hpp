#pragma once
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <limits>

#include "vec3d.hpp"
#include "Triangle.hpp"
#include "Sphere.hpp"
#include "Ray.hpp"
#include "utils.hpp"


class GameObject {
public:
	GameObject(std::string filename) {
        parser(filename);
        std::cout << filename << ' ' << triagles.size() << ' ' << verteces.size();
        sph = smallest_sphere(verteces);
	}

    double is_hitted(const Ray& inRay) {
        double final_res = 0;
        double last_dist = DBL_MAX;

        if (!sph.is_hitted(inRay))
            return 0;

        for (auto& tr : triagles) {
            double res = tr.is_hitted(inRay);
            if (res) {
                if (last_dist > tr.last_dist){
                    final_res = res;
                    last_dist = tr.last_dist;
                }
                    
            }
        }

        return final_res;
    }

    void rotate(const double a, const double b, const double g) {
        vec3d<double> center = sph.center;
        for (auto& v : verteces) {
            v = ROTATE(v - center, a, b, g) + center;
        }
        for (auto& t : triagles) {
            auto[p1, p2, p3] = t.get_points();
            t = Triangle(
                ROTATE(p1 - center, a, b, g) + center,
                ROTATE(p2 - center, a, b, g) + center,
                ROTATE(p3 - center, a, b, g) + center
            );
        }
    }

    void scale(const double n) {
        vec3d<double> center = sph.center;
        for (auto& v : verteces) {
            v = (v - center) * n + center;
        }
        for (auto& t : triagles) {
            auto [p1, p2, p3] = t.get_points();
            t = Triangle(
                (p1 - center) * n + center,
                (p2 - center) * n + center,
                (p3 - center) * n + center
            );
        }
        sph.radius *= n;
    }

    void move(const vec3d<double>& m_vec) {
        for (auto& v : verteces) {
            v = v + m_vec;
        }
        for (auto& t : triagles) {
            auto [p1, p2, p3] = t.get_points();
            t = Triangle(p1 + m_vec, p2 + m_vec, p3 + m_vec);
        }
        sph.center = sph.center + m_vec;
    }

private:
	std::vector<Triangle> triagles;
    std::vector<vec3d<double>> verteces;
    Sphere sph;

    // Find the smallest sphere that encloses a set of points
    Sphere smallest_sphere(const std::vector<vec3d<double>>& points) {
        int n = points.size();
        if (n == 0) {
            Sphere s;
            s.center = { 0.0, 0.0, 0.0 };
            s.radius = 0.0;
            return s;
        }
        if (n == 1) {
            Sphere s;
            s.center = points[0];
            s.radius = 0.0;
            return s;
        }
        if (n == 2) {
            Sphere s;
            s.center = { (points[0].x + points[1].x) / 2.0, (points[0].y + points[1].y) / 2.0, (points[0].z + points[1].z) / 2.0 };
            s.radius = dist(points[0], points[1]) / 2.0;
            return s;
        }
        Sphere s;
        s.center = { 0.0, 0.0, 0.0 };
        s.radius = std::numeric_limits<double>::infinity();
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                for (int k = j + 1; k < n; k++) {
                    vec3d<double> p1 = points[i];
                    vec3d<double> p2 = points[j];
                    vec3d<double> p3 = points[k];
                    double a = 2.0 * (p2.x - p1.x);
                    double b = 2.0 * (p2.y - p1.y);
                    double c = 2.0 * (p2.z - p1.z);
                    double d = 2.0 * (p3.x - p1.x);
                    double e = 2.0 * (p3.y - p1.y);
                    double f = 2.0 * (p3.z - p1.z);
                    double g = p2.x * p2.x + p2.y * p2.y + p2.z * p2.z - p1.x * p1.x - p1.y * p1.y - p1.z * p1.z;
                    double h = p3.x * p3.x + p3.y * p3.y + p3.z * p3.z - p1.x * p1.x - p1.y * p1.y - p1.z * p1.z;
                    double det = a * e * f + b * f * d + c * d * e - a * f * e - b * d * d - c * b * f;
                    if (abs(det) > 1e-6) {
                        vec3d<double> center;
                        center.x = (g * e * f + h * b * f + c * d * h - g * f * e - b * d * h - center.x) / det;
                        center.y = (a * h * f + g * d * f + c * e * h - a * f * h - g * e * c - c * a * d) / det;
                        center.z = (a * e * h + b * d * h + g * b * e - a * b * f - d * b * g - c * e * g) / det;
                        double radius = dist(points[i], center);
                        bool inside = true;
                        for (int l = 0; l < n; l++) {
                            if (dist(points[l], center) > radius) {
                                inside = false;
                                break;
                            }
                        }
                        if (inside && radius < s.radius) {
                            s.center = center;
                            s.radius = radius;
                        }
                    }
                }
            }
        }
        return s;
    }

    void parser(const std::string& filename) {
        double max_x = -DBL_MAX;
        double min_x = DBL_MAX;
        double max_y = -DBL_MAX;
        double min_y = DBL_MAX;
        std::ifstream in(filename, std::ios::in);
        if (!in)
            throw "Cannot open " + filename + "\n";

        std::string line;
        while (std::getline(in, line))
        {
            std::string diff_s = "";
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
};
