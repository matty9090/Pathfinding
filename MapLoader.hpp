#pragma once

#include <vector>
#include <string>

#include "Vec3.hpp"

/*
	Class to load and a map file
*/

class CMapLoader
{
	public:
		CMapLoader();

		void SetDims(size_t w, size_t h) { mWidth = w, mHeight = h; }

		std::vector<std::vector<int>> Load(std::string f);
		std::pair<Vec2<>, Vec2<>> Coords(std::string f);

	private:
		size_t mWidth, mHeight;

		void AllocSpace(std::vector<std::vector<int>> &v);
};