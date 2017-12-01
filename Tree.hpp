#pragma once

#include <memory>
#include <vector>

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

	private:
		std::vector<std::vector<Node>> &grid;

		size_t w, h;
};