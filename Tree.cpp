#include "Tree.hpp"

using namespace std;

Tree::Tree(size_t _w, size_t _h) : w(_w), h(_h) {
	grid.resize(h);

	for (size_t i = 0; i < h; ++i)
		grid[i].resize(w);
}

Tree::Node Tree::setNode(unsigned x, unsigned y, Vec3<> pos, float cost) {
	Node n = make_shared<_Node>(pos, cost);
	grid[y][x] = n;

	return n;
}

Tree::Node Tree::getNode(unsigned x, unsigned y) {
	return grid[y][x];
}

Tree::Node Tree::findNode(Vec3<> pos, float max_d) {
	for (auto rows : grid) {
		for (auto node : rows) {
			if (node->pos.distance(pos) < max_d)
				return node;
		}
	}

	return nullptr;
}
