#include "StateMenu.hpp"
#include "MapLoader.hpp"

#include <cassert>

using namespace tle;

StateMenu::StateMenu(tle::I3DEngine *engine, Settings &settings) : State(engine, settings) {
	
}

void StateMenu::init() {
	
}

int StateMenu::run() {
	while (engine->IsRunning()) {
		if (engine->KeyHit(Key_Escape)) {
			engine->Stop();
			break;
		}

		engine->DrawScene();
	}

	return State::Exit;
}