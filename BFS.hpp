#pragma once

#include "Tree.hpp"

class BFS : public SearchAlgorithm {
	public:
		BFS(Tree &tree);

		void start(Tree::Node start, Tree::Node goal);
		int step();

	private:
		std::map<Tree::Node, Tree::Node> data;

		bool useDiag;
		bool goal_found;

		void construct_path();
};