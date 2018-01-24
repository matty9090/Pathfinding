#pragma once

#include <string>

/*
	2D vector class
*/
template <class T = int> struct Vec2 {
	T x, y;

	Vec2() : x(0), y(0) {}
	Vec2(T _x, T _y) : x(_x), y(_y) {}

	inline Vec2 operator+(Vec2 v) {
		return Vec2(x + v.x, y + v.y);
	}

	inline Vec2 operator-(Vec2 v) {
		return Vec2(x - v.x, y - v.y);
	}

	inline Vec2 operator/(float scale) {
		return Vec2(x / scale, y / scale);
	}

	std::string toString() {
		return "(" + to_string(x) + ", " + to_string(y) + ")";
	}
};

/*
	3D vector class
*/
template <class T = float> struct Vec3 {
	T x, y, z;
	
	Vec3() : x(0), y(0), z(0) {}
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}

	inline float distance(Vec3 v) {
		T dx = (v.x - x) * (v.x - x);
		T dy = (v.y - y) * (v.y - y);
		T dz = (v.z - z) * (v.z - z);

		return sqrtf(dx + dy + dz);
	}

	inline Vec3 operator+(Vec2<> v) {
		return Vec3(v.x, 0, v.y);
	}

	inline Vec3 operator*(Vec3 v) {
		return Vec3(y * v.getZ() - z * v.getY(),
					z * v.getX() - x * v.getZ(),
					x * v.getY() - y * v.getX());
	}

	std::string toString() {
		return "( " + to_string(x) + ", " + to_string(y) + ", " + to_string(z) + ")";
	}
};
