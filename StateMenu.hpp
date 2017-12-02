#pragma once

#include <TL-Engine.h>
#include <vector>
#include <map>
#include <memory>

#include "Settings.hpp"
#include "State.hpp"

class StateMenu : public State {
	public:
		StateMenu(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

	private:
	
};