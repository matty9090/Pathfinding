#pragma once

#include <string>
#include <map>

#include "JSON.hpp"

class Settings {
	public:
		Settings(std::string file);

		size_t getScreenW() { return window_w; }
		size_t getScreenH() { return window_h; }
		std::string getMapsFolder() { return maps_folder; }

		struct Model {
			std::string file, tex;
		};

		std::map<std::string, Model> getModels() { return models; }

	private:
		picojson::value json;

		size_t window_w, window_h;
		std::string maps_folder;
		
		std::map<std::string, Model> models;

		void load_properties();
		void load_models();
};