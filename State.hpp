#pragma once

#include <TL-Engine.h>

class State {
	public:
		State(tle::I3DEngine *_engine, Settings &_settings) : engine(_engine), settings(_settings) {}

		virtual void init() = 0;
		virtual int run() = 0;

		enum States { Pathfinder, Exit };

	protected:
		Settings &settings;
		tle::I3DEngine *engine;
};