#pragma once

#include <TL-Engine.h>
#include <vector>
#include <map>
#include <memory>

#include "Settings.hpp"
#include "State.hpp"

class StatePathfinder : public State {
	public:
		StatePathfinder(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

		void load_maps();
		void load_models();
		void free_memory();

	private:
		Vec2<> start, goal;
		Vec2<size_t> dims;

		tle::ICamera *cam;

		std::map<std::string, tle::IMesh*> meshes;
		std::map<std::string, tle::IModel*> models;

		struct NodeMap {
			std::vector<std::vector<int>> map;
			std::vector<std::vector<tle::IModel*>> model;

			void constructMap(Vec3<> origin);
		} map;
};