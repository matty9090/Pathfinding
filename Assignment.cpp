#include <string>

#include "Settings.hpp"
#include "App.hpp"

// Only global variable needed is the path to the settings file
const std::string gSettingsFile = "settings.dat";

void main() {
	// Load in the settings
	CSettings settings(gSettingsFile);

	// Run the application
	CApp app(settings);
	app.run();
}
