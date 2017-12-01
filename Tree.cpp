#include "Tree.hpp"

using namespace std;

Tree::Tree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid) : w(dims.x), h(dims.y), grid(_grid) {
	
}

Tree::Node Tree::setNode(unsigned x, unsigned y, Vec2<> pos, int cost) {
	Node n = make_shared<_Node>(pos, cost);
	grid[y][x] = n;

	return n;
}

Tree::Node Tree::getNode(unsigned x, unsigned y) {
	return grid[y][x];
}

Tree::Node Tree::findNode(Vec2<> pos) {
	for (auto rows : grid) {
		for (auto node : rows) {
			if (node->pos.x == pos.x && node->pos.y == pos.y)
				return node;
		}
	}

	return nullptr;
}
