#include "Settings.hpp"

#include <fstream>

using namespace std;

Settings::Settings(string file) {
	ifstream f(file);
	picojson::parse(json, f);

	load_properties();
	load_models();
	load_maps();

	f.close();

	current_map = "Map 1";
}

void Settings::load_properties() {
	window_w = (size_t)json.get("window_w").get<double>();
	window_h = (size_t)json.get("window_h").get<double>();

	map_scale	= (float)json.get("map_scale").get<double>();
	maps_folder	= json.get("maps_folder").get<string>();
}

void Settings::load_models() {
	picojson::array model_list = json.get("models").get<picojson::array>();

	cout << "Models:\n\n";

	for (auto model : model_list) {
		string name = model.get("name").get<string>();
		string file = model.get("file").get<string>();
		string tex  = "";
		float scale = 1.0f;

		cout << "\t" << name << " (" << file << ")";
		
		if (!model.get("tex").is<picojson::null>()) {
			tex = model.get("tex").get<string>();
			cout << " using texture " << tex;
		}

		if (!model.get("scale").is<picojson::null>()) {
			scale = (float)model.get("scale").get<double>();
			cout << " scaled to " << scale;
		}

		if (!model.get("x").is<picojson::null>()) {
			float x = (float)model.get("x").get<double>();
			float y = (float)model.get("y").get<double>();
			float z = (float)model.get("z").get<double>();

			models[name].inst = true;
			models[name].pos  = Vec3<>(x, y, z);
		}
		
		models[name].tex = tex;
		models[name].file = file;
		models[name].scale = scale;

		cout << "\n";
	}

	cout << "\n";
}

void Settings::load_maps() {
	picojson::array map_list = json.get("maps").get<picojson::array>();

	cout << "Maps:\n\n";

	for (auto model : map_list) {
		string name  = model.get("name").get<string>();
		string mfile = model.get("map_file").get<string>();
		string cfile = model.get("coords_file").get<string>();
		size_t map_w = (size_t)model.get("map_width").get<double>();
		size_t map_h = (size_t)model.get("map_height").get<double>();

		Vec2<size_t> dims(map_w, map_h);

		maps[name].map_file = mfile;
		maps[name].coords_file = cfile;
		maps[name].dims = dims;

		cout << "\t" << name << " " << dims.toString();
		cout << " - " << mfile << ", " << cfile << "\n";
	}

	cout << "\n";
}