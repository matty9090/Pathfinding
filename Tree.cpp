#include "Tree.hpp"

#include <deque>
#include <set>
#include <map>
#include <iostream>

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

// Manhattan Distance
float SearchAlgorithm::heuristic(Tree::Node start, Tree::Node end) {
	return (float)abs(start->pos.x - end->pos.x) + (float)abs(start->pos.y - end->pos.y);
}
