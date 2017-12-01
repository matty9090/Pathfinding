#pragma once

#include <TL-Engine.h>

class State {
	public:
		virtual void init(tle::I3DEngine *engine, Settings &settings) = 0;
		virtual int run() = 0;

		static enum States { Pathfinder, Exit };
};