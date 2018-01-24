#pragma once

#include "Tree.hpp"

class AStar : public SearchAlgorithm {
	public:
		AStar(Tree &tree);

		void start(Tree::Node start, Tree::Node goal);
		int step();

	private:
		std::map<Tree::Node, Tree::Node> data;

		bool goal_found;

		void construct_path();
};