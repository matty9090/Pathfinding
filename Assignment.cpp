#include <string>

#include "Settings.hpp"
#include "App.hpp"

using namespace tle;

const std::string settings_file = "settings.dat";

void main() {
	Settings settings(settings_file);

	App app(1024U, 768U);
	app.run();
}
