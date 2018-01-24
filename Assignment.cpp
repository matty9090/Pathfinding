#include <string>

#include "Settings.hpp"
#include "App.hpp"

using namespace tle;

const std::string gSettingsFile = "settings.dat";

void main() {
	CSettings settings(gSettingsFile);

	CApp app(settings);
	app.run();
}
