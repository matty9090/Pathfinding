#include "App.hpp"

using namespace tle;

App::App(size_t w, size_t h) {
	eng = New3DEngine(kTLX);

	eng->StartWindowed(w, h);
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
