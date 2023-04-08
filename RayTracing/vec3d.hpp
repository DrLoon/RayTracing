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
	T lenght() {
		return sqrt(x * x + y * y + z * z);
	}
	void normalize() {
		T len = this->lenght();
		if (!len) throw("ded already inside");
		x /= len;
		y /= len;
		z /= len;
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
vec3d<T> operator+ (const vec3d<T>& vec1, const vec3d<T>& vec2) {
	return vec3d<T>(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

template<typename T>
vec3d<T> operator- (const vec3d<T>& vec1, const vec3d<T>& vec2) {
	return vec3d<T>(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
}

template<typename T>
vec3d<T> cross_product(vec3d<T>& vec1, vec3d<T>& vec2) {
	T i = vec1.y * vec2.z - vec2.y * vec1.z;
	T j = vec2.x * vec1.z - vec1.x * vec2.z;
	T k = vec1.x * vec2.y - vec2.x * vec1.y;
	return vec3d<T>(i, j, k);
}
template<typename T>
vec3d<T> cross_product(vec3d<T>&& vec1, vec3d<T>&& vec2) {
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
vec3d<T> rotateByAxis(vec3d<T>& axis, vec3d<T>& vec, T angle) {
	axis.normalize();
	return vec * cos(angle) + cross_product(axis, vec) * sin(angle) + axis * (axis * vec) * (1 - cos(angle));
}

template<typename T>
T dist(const vec3d<T>& p1, const vec3d<T>& p2) {
	return sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2) + std::pow(p1.z - p2.z, 2));
}

template class vec3d<float>;
template class vec3d<double>;