#pragma once

#include <string>
#include <map>
#include <TL-Engine.h>

#include "JSON.hpp"
#include "Vec3.hpp"

class Settings {
	public:
		Settings(std::string file);

		void setMap(std::string map_id) { current_map = map_id; }
		size_t getScreenW() { return window_w; }
		size_t getScreenH() { return window_h; }
		float getMapScale() { return map_scale; }
		std::string getMapsFolder() { return maps_folder; }

		struct Model {
			std::string file, tex;
			Vec3<> pos;
			bool inst;
			float scale;
		};

		struct Map {
			std::string map_file, coords_file;
			Vec2<size_t> dims;
		};

		struct KeyDesc {
			std::string desc;
			tle::EKeyCode code;
		};

		std::map<std::string, Model> &getModels() { return models; }
		std::map<std::string, Map> &getMaps() { return maps; }
		std::map<std::string, KeyDesc> &getKeys() { return keys; }
		tle::EKeyCode getKeyCode(std::string txt) { return keys[txt].code; }

		Map &currentMap() { return maps[current_map]; }

	private:
		picojson::value json;

		float map_scale;
		size_t window_w, window_h;
		std::string maps_folder, current_map;
		
		std::map<std::string, Model> models;
		std::map<std::string, Map> maps;
		std::map<std::string, KeyDesc> keys;

		void load_properties();
		void load_models();
		void load_maps();
		void load_keys();
};