#include "StatePathfinder.hpp"

using namespace tle;

StatePathfinder::StatePathfinder(tle::I3DEngine *engine, Settings &settings) : State(engine, settings) {

}

void StatePathfinder::init() {
	
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
