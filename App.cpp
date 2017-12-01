#include "App.hpp"

using namespace tle;

App::App(Settings &s) : settings(s) {
	eng = New3DEngine(kTLX);

	eng->StartWindowed(settings.getScreenW(), settings.getScreenH());
	eng->AddMediaFolder("./res");
}

App::~App() {
	eng->Delete();
}

void App::run() {
	while (eng->IsRunning()) {
		eng->DrawScene();
	}
}
