#pragma once

#include <memory>
#include <vector>

#include "Vec3.hpp"

struct _Node {
	Vec3<> pos;
	float cost;

	_Node(Vec3<> p, float c) : pos(p), cost(c) {};
};

class Tree {
	public:
		typedef shared_ptr<_Node> Node;

		Tree(size_t w, size_t h);

		Node setNode(unsigned x, unsigned y, Vec3<> pos, float cost = 1.0f);
		Node getNode(unsigned x, unsigned y);
		Node findNode(Vec3<> pos, float max_d = 1.0f);

	private:
		std::vector<std::vector<Node>> grid;

		size_t w, h;
};