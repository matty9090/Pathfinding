#pragma once

#include "Tree.hpp"

class AStar : public SearchAlgorithm {
	public:
		AStar(Tree &tree);

		void start(Tree::Node start, Tree::Node goal);
		int step();

	private:
		std::map<Tree::Node, Tree::Node> data;
		std::map<Tree::Node, float> g_score;
		std::map<Tree::Node, float> f_score;

		float heuristic(Tree::Node start, Tree::Node end);
};