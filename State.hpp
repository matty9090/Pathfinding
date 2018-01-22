#pragma once

#include <TL-Engine.h>

#include "Settings.hpp"

class State {
	public:
		State(tle::I3DEngine *_engine, Settings &_settings) : engine(_engine), settings(_settings) {}

		virtual void init() = 0;
		virtual int run() = 0;

		enum States { Menu, MapChooser, Pathfinder, Exit };

	protected:
		Settings &settings;
		tle::I3DEngine *engine;
};