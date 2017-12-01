#pragma once

#include <TL-Engine.h>
#include <map>#

#include "Settings.hpp"

class App {
	public:
		App(Settings &settings);
		~App();

		void run();

	private:
		Settings &settings;
		tle::I3DEngine *eng;

		std::map<string, tle::IModel*> models;
};