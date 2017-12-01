#include "App.hpp"

using namespace tle;

App::App(Settings &s) : settings(s) {
	eng = New3DEngine(kTLX);

	eng->StartWindowed(settings.getScreenW(), settings.getScreenH());
	eng->AddMediaFolder("./res");

	state = State::Pathfinder;

	states.push_back(make_unique<StatePathfinder>());
}

App::~App() {
	eng->Delete();
}

void App::run() {
	do {
		states[state]->init(eng, settings);
		state = states[state]->run();
	} while (state != State::Exit);
}
