#pragma once

#include <vector>
#include <string>

#include "Vec3.hpp"

class MapLoader {
	public:
		MapLoader();

		void setDims(size_t _w, size_t _h) { w = _w, h = _h; }

		std::vector<std::vector<int>> load(std::string f);
		std::pair<Vec2<>, Vec2<>> coords(std::string f);

	private:
		size_t w, h;

		void alloc_space(std::vector<std::vector<int>> &v);
};