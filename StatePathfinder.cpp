#include "StatePathfinder.hpp"

using namespace tle;

void StatePathfinder::init(I3DEngine *_engine) {
	engine = _engine;
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
