#pragma once

#include <TL-Engine.h>
#include <vector>
#include <map>
#include <list>
#include <memory>

#include "Settings.hpp"
#include "State.hpp"
#include "Tree.hpp"
#include "BFS.hpp"
#include "AStar.hpp"

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

		std::shared_ptr<SearchAlgorithm> searcher;

		Vec2<size_t> dims;

		bool found;
		int pathNum;
		float scale, timer;
		Vec3<> origin;

		tle::ICamera *cam;

		std::vector<tle::IModel*> search_path;
		std::vector<std::vector<tle::IModel*>> path;

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

		float cspline(float p1, float p2, float p3, float p4, float t);
		float bspline(float p1, float p2, float p3, float p4, float t);
		float lerp(float v0, float v1, float t);

		void clearPathSearch();
		void displayPathSearch(std::set<Tree::Node> open, std::set<Tree::Node> closed);
		void displayPathBezier(std::vector<Vec2<>> path, std::string id);
		void displayPathCatmullRom(std::vector<Vec2<>> path, std::string id);
};