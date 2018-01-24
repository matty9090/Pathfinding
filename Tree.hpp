#pragma once

#include <memory>
#include <vector>
#include <list>
#include <set>
#include <map>

#include "Vec3.hpp"

struct _Node {
	Vec2<> pos;
	int cost;
	int score;
	int estimate;

	_Node(Vec2<> p, int c) : pos(p), cost(c) {};
};

class Tree {
	public:
		typedef std::shared_ptr<_Node> Node;

		Tree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid);

		Node setNode(unsigned x, unsigned y, Vec2<> pos, int cost = 1);
		Node getNode(unsigned x, unsigned y);
		Node findNode(Vec2<> pos);

		Vec2<size_t> getGridSize() { return Vec2<size_t>(w, h); }

	private:
		std::vector<std::vector<Node>> &grid;

		size_t w, h;
};

class SearchAlgorithm {
	public:
		enum Status { Found, Searching, Failed };

		SearchAlgorithm(Tree &t, bool diag = false) : tree(t), useDiag(diag) {}

		virtual void start(Tree::Node start, Tree::Node goal) = 0;
		virtual int step() = 0;
		
		std::vector<Vec2<>> getPath() { return path; }

		int GetNumSearches() { return mNumSearches; }
		void useDiagonals(bool diag) { useDiag = diag; }

		std::set<Tree::Node> getOpenList() { return open; }
		std::set<Tree::Node> getClosedList() { return closed; }

	protected:
		Tree &tree;
		Tree::Node startn, goaln;

		std::vector<Vec2<>> path;
		std::set<Tree::Node> open;
		std::set<Tree::Node> closed;

		int mNumSearches;
		bool useDiag;

		int heuristic(Tree::Node start, Tree::Node end);
};