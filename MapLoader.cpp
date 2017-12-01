#include "MapLoader.hpp"

#include <cassert>
#include <fstream>
#include <iostream>

using namespace std;

MapLoader::MapLoader() {

}

std::vector<std::vector<int>> MapLoader::load(std::string map_file) {
	assert(w > 0 && h > 0);

	ifstream mapf(map_file);
	std::vector<std::vector<int>> v;

	if (mapf.is_open()) {
		alloc_space(v);

		for (unsigned y = 0; y < h; ++y) {
			for (unsigned x = 0; x <= w; ++x) {
				char c;
				mapf.get(c);

				// Skip end of line character
				if (x >= w)
					continue;
				else
					v[h - y - 1][x] = (int)(c - '0');
			}
		}
	}

	mapf.close();

	return v;
}

std::pair<Vec2<>, Vec2<>> MapLoader::coords(std::string coords_file) {
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

void MapLoader::alloc_space(std::vector<std::vector<int>>& v) {
	v.resize(h);

	for (unsigned i = 0; i < h; ++i)
		v[i].resize(w);
}
