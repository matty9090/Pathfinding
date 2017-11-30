#pragma once

template <class T> struct Vec3 {
	T x, y, z;
	
	Vec3() : x(0), y(0), z(0) {};
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {};
};