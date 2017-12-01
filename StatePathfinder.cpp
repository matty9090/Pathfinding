#include "StatePathfinder.hpp"

using namespace tle;

void StatePathfinder::init(I3DEngine *_engine, Settings &_settings) {
	engine = _engine;
	settings = _settings;
}

int StatePathfinder::run() {
	while (engine->IsRunning()) {
		if (engine->KeyHit(Key_Escape)) {
			engine->Stop();
			break;
		}

		engine->DrawScene();
	}

	return State::Exit;
}
