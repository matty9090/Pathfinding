#pragma once

template <class T = float> struct Vec3 {
	T x, y, z;
	
	Vec3() : x(0), y(0), z(0) {};
	Vec3(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {};

	inline float distance(Vec3 v) {
		T dx = (v.x - x) * (v.x - x);
		T dy = (v.y - y) * (v.y - y);
		T dz = (v.z - z) * (v.z - z);

		return sqrtf(dx + dy + dz);
	}
};
