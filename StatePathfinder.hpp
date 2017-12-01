#pragma once

#include <TL-Engine.h>

#include "Settings.hpp"
#include "State.hpp"

class StatePathfinder : public State {
	public:
		StatePathfinder(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

	private:

};