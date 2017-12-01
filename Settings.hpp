#pragma once

#include <string>
#include <map>

#include "JSON.hpp"
#include "Vec3.hpp"

class Settings {
	public:
		Settings(std::string file);

		size_t getScreenW() { return window_w; }
		size_t getScreenH() { return window_h; }
		std::string getMapsFolder() { return maps_folder; }

		struct Model {
			std::string file, tex;
		};

		struct Map {
			std::string map_file, coords_file;
			Vec2<size_t> dims;
		};

		std::map<std::string, Model> &getModels() { return models; }
		std::map<std::string, Map> &getMaps() { return maps; }

	private:
		picojson::value json;

		size_t window_w, window_h;
		std::string maps_folder;
		
		std::map<std::string, Model> models;
		std::map<std::string, Map> maps;

		void load_properties();
		void load_models();
		void load_maps();
};