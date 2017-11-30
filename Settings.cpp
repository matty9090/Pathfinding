#include "Settings.hpp"

#include <fstream>

using namespace std;

Settings::Settings(string file) {
	ifstream f(file);
	picojson::parse(json, f);

	load_properties();

	f.close();
}

void Settings::load_properties() {
	window_w = (size_t)json.get("window_w").get<double>();
	window_h = (size_t)json.get("window_h").get<double>();
}
