#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <vector>

#include "State.hpp"
#include "Settings.hpp"

class App {
	public:
		App(Settings &settings);
		~App();

		void run();

	private:
		int state;
		std::vector<std::unique_ptr<State>> states;

		Settings &settings;
		tle::I3DEngine *eng;

		std::map<string, tle::IModel*> models;
};