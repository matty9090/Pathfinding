#pragma once

#include <TL-Engine.h>

#include "Settings.hpp"

class CState {
	public:
		CState(tle::I3DEngine *engine, CSettings &settings) : mpEngine(engine), mSettings(settings) {}

		virtual void Init() = 0;
		virtual int Run() = 0;

		enum EStates { Menu, MapChooser, Pathfinder, Exit };

	protected:
		CSettings &mSettings;
		tle::I3DEngine *mpEngine;
};