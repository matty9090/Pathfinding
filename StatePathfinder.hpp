#pragma once

#include <TL-Engine.h>
#include <vector>
#include <map>
#include <memory>

#include "Settings.hpp"
#include "State.hpp"
#include "Tree.hpp"

class StatePathfinder : public State {
	public:
		StatePathfinder(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

		void load_maps();
		void load_models();
		void free_memory();

	private:
		Tree tree;
		Tree::Node start, goal;
		Vec2<size_t> dims;

		tle::ICamera *cam;

		std::map<std::string, tle::IMesh*> meshes;
		std::map<std::string, tle::IModel*> models;

		std::map<int, std::pair<std::string, int>> node_types;

		struct NodeMap {
			std::vector<std::vector<Tree::Node>> map;
			std::vector<std::vector<tle::IModel*>> models;

			StatePathfinder &parent;

			NodeMap(StatePathfinder &_parent) : parent(_parent) {}

			void constructMap(Vec3<> origin, float scale);
			inline Vec3<> translate(Vec2<> coord, Vec3<> &origin, float scale);
		} map;
};