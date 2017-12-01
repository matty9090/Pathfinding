#pragma once

#include <TL-Engine.h>

#include "Settings.hpp"
#include "State.hpp"

class StatePathfinder : public State {
	public:
		void init(tle::I3DEngine *engine, Settings &settings);
		int run();

	private:
		Settings &settings;
		tle::I3DEngine *engine;
};