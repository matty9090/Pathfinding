#include "Settings.hpp"

#include <fstream>

using namespace std;

Settings::Settings(string file) {
	ifstream f(file);
	picojson::parse(json, f);

	load_properties();
	load_models();

	f.close();
}

void Settings::load_properties() {
	window_w = (size_t)json.get("window_w").get<double>();
	window_h = (size_t)json.get("window_h").get<double>();

	maps_folder = json.get("maps_folder").get<string>();
}

void Settings::load_models() {
	picojson::array model_list = json.get("models").get<picojson::array>();

	cout << "Models:\n\n";

	for (auto model : model_list) {
		string name = model.get("name").get<string>();
		string file = model.get("file").get<string>();
		string tex  = "";

		cout << name << " (" << file << ")";
		
		if (!model.get("tex").is<picojson::null>()) {
			tex = model.get("tex").get<string>();
			cout << " using texture " << tex;
		}

		models[name].file = file;
		models[name].tex = tex;

		cout << "\n";
	}

	cout << "\n";
}
