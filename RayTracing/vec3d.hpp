#pragma once
#include <iostream>
//#define double float
#define PI 3.14159265358979323846

template<typename T>
class vec3d {
public:
	T x, y, z;
	vec3d(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	vec3d(const vec3d<T>& in_vec) : x(in_vec.x), y(in_vec.y), z(in_vec.z) {}
	vec3d() : x(0), y(0), z(0) {}
	T lenght() const {
		return sqrt(x * x + y * y + z * z);
	}
	void normalize() {
		T len = this->lenght();
		x /= len;
		y /= len;
		z /= len;
	}
	vec3d<double> normalize() const {
		T len = this->lenght();
		return vec3d<double>(x / len, y / len, z / len);
	}
};

template<typename T>
T operator* (const vec3d<T>& vec1, const vec3d<T>& vec2) {
	return vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
}
template<typename T>
vec3d<T> operator* (const T scalar, const vec3d<T>& vec) {
	return vec3d<T>(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}
template<typename T>
vec3d<T> operator* (const vec3d<T>& vec, const T scalar) {
	return vec3d<T>(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}

template<typename T>
vec3d<T> operator/ (const vec3d<T>& vec, const T scalar) {
	return vec * (1.0 / scalar);
}

template<typename T>
vec3d<T> operator+ (const vec3d<T>& vec1, const vec3d<T>& vec2) {
	return vec3d<T>(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

template<typename T>
vec3d<T> operator- (const vec3d<T>& vec1, const vec3d<T>& vec2) {
	return vec3d<T>(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

template<typename T>
vec3d<T> cross_product(const vec3d<T>& vec1, const vec3d<T>& vec2) {
	T i = vec1.y * vec2.z - vec2.y * vec1.z;
	T j = vec2.x * vec1.z - vec1.x * vec2.z;
	T k = vec1.x * vec2.y - vec2.x * vec1.y;
	return vec3d<T>(i, j, k);
}
template<typename T>
vec3d<T> cross_product(const vec3d<T>&& vec1, const vec3d<T>&& vec2) {
	T i = vec1.y * vec2.z - vec2.y * vec1.z;
	T j = vec2.x * vec1.z - vec1.x * vec2.z;
	T k = vec1.x * vec2.y - vec2.x * vec1.y;
	return vec3d<T>(i, j, k);
}

template<typename T>
std::ostream& operator<<(std::ostream& stream, const vec3d<T>& vec) {
	stream << vec.x << " " << vec.y << " " << vec.z << "\n";
	return stream;
}

template<typename T>
vec3d<T> rotateByAxis(vec3d<T>& axis, const vec3d<T>& vec, const T angle) {
	// changes axis, not const function!
	axis.normalize();
	return vec * cos(angle) + cross_product(axis, vec) * sin(angle) + axis * (axis * vec) * (1 - cos(angle));
}
template<typename T>
vec3d<T> rotateByAxis(const vec3d<T>& axis, const vec3d<T>& vec, const T angle) {
	vec3d<T> n = axis.normalize();
	return vec * cos(angle) + cross_product(n, vec) * sin(angle) + n * (n * vec) * (1 - cos(angle));
}

template<typename T>
T dist(const vec3d<T>& p1, const vec3d<T>& p2) {
	return sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) + std::pow(p1.z - p2.z, 2));
}

template<typename T>
vec3d<T> ROTATE(const vec3d<T>& vec, const double a, const double b, const double g) {
	const T x = vec.x * cos(b) * cos(g) - vec.y * sin(g) * cos(b) + vec.z * sin(b);
	const T y = vec.x * (sin(a) * sin(b) * cos(g) + sin(g) * cos(a)) + vec.y * (-sin(a) * sin(b) * sin(g) + cos(a) * cos(g)) - vec.z * sin(a) * cos(b);
	const T z = vec.x * (sin(a) * sin(g) - sin(b) * cos(a) * cos(g)) + vec.y * (sin(a) * cos(g) + sin(b) * sin(g) * cos(a)) + vec.z * cos(a) * cos(b);
	return vec3d<T>(x, y, z);
}

template class vec3d<float>;
template class vec3d<double>;
