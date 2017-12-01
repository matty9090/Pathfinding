#pragma once

#include <vector>
#include <string>

class MapLoader {
	public:
		MapLoader();

		void setDims(size_t _w, size_t _h) { w = _w, h = _h; }
		void setMapFile(std::string f) { map_file = f; }
		void setCoordsFile(std::string f) { coords_file = f; }

		std::vector<std::vector<int>> load();

	private:
		size_t w, h;
		std::string map_file, coords_file;

		void alloc_space(std::vector<std::vector<int>> &v);
};