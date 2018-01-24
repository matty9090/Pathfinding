#include "MapLoader.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

CMapLoader::CMapLoader() {

}

std::vector<std::vector<int>> CMapLoader::Load(std::string map_file) {
	assert(mWidth > 0 && mHeight > 0);

	ifstream mapf(map_file);
	std::vector<std::vector<int>> v;

	if (mapf.is_open()) {
		AllocSpace(v);

		for (unsigned y = 0; y < mHeight; ++y) {
			for (unsigned x = 0; x <= mWidth; ++x) {
				char c;
				mapf.get(c);

				// Skip end of line character
				if (x >= mWidth)
					continue;
				else
					v[mHeight - y - 1][x] = (int)(c - '0');
			}
		}
	}

	mapf.close();

	return v;
}

std::pair<Vec2<>, Vec2<>> CMapLoader::Coords(std::string coords_file) {
	Vec2<> start, goal;

	ifstream coordf(coords_file);

	if (coordf.is_open()) {
		int x0, y0, x1, y1;

		coordf >> x0 >> y0 >> x1 >> y1;

		start.x = x0, start.y = y0;
		goal.x  = x1, goal.y  = y1;
	}

	coordf.close();

	return make_pair(start, goal);
}

void CMapLoader::AllocSpace(std::vector<std::vector<int>>& v) {
	v.resize(mHeight);

	for (unsigned i = 0; i < mHeight; ++i)
		v[i].resize(mWidth);
}
