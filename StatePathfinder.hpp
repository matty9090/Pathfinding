#pragma once

#include <TL-Engine.h>

#include "State.hpp"

class StatePathfinder : public State {
	public:
		void init(tle::I3DEngine *engine);
		int run();

	private:
		tle::I3DEngine *engine;
};