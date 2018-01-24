/// Author: Matthew Lowe
/// Copyright (c) 2018

#pragma once

#include <TL-Engine.h>

#include "Settings.hpp"

/*
	Abstract state class
	To be derived from
*/

class CState
{
	public:
		CState(tle::I3DEngine *engine, CSettings &settings) : mpEngine(engine), mSettings(settings) {}

		virtual void Init() = 0;
		virtual int Run() = 0;

		// Store all possible states in an enum
		enum EStates { Menu, MapChooser, Pathfinder, Exit };

	protected:
		CSettings &mSettings;
		tle::I3DEngine *mpEngine;
};