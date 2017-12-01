#pragma once

#include <TL-Engine.h>
#include <vector>

#include "Settings.hpp"
#include "State.hpp"

class StatePathfinder : public State {
	public:
		StatePathfinder(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

		void free_memory();

	private:
		Vec2<> start, goal;
		Vec2<size_t> dims;

		std::vector<std::vector<int>> map;
};