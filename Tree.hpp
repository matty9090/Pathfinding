#pragma once

#include <memory>
#include <vector>
#include <list>

#include "Vec3.hpp"

struct _Node {
	Vec2<> pos;
	int cost;

	_Node(Vec2<> p, int c) : pos(p), cost(c) {};
};

class Tree {
	public:
		typedef std::shared_ptr<_Node> Node;

		Tree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid);

		Node setNode(unsigned x, unsigned y, Vec2<> pos, int cost = 1);
		Node getNode(unsigned x, unsigned y);
		Node findNode(Vec2<> pos);

		std::vector<Vec2<>> pathfind_bfs(Node start, Node goal);
		std::vector<Vec2<>> pathfind_astar(Node start, Node goal);

		float heuristic(Node start, Node end);

	private:
		std::vector<std::vector<Node>> &grid;

		size_t w, h;
};