#include "Tree.hpp"

#include <deque>
#include <set>
#include <map>
#include <iostream>

using namespace std;

// Initialise values passed in from constructor
CTree::CTree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid) : mWidth(dims.x), mHeight(dims.y), mGrid(_grid) {
	
}

// Create and return a pointer to a new node with a particular position and cost
CTree::Node CTree::SetNode(unsigned x, unsigned y, Vec2<> mPos, int mCost) {
	Node n = make_shared<SNode>(mPos, mCost);
	mGrid[y][x] = n;

	return n;
}

// Get a node from the grid
CTree::Node CTree::GetNode(unsigned x, unsigned y) {
	return mGrid[y][x];
}

// Find a node with the specified coordinates
CTree::Node CTree::FindNode(Vec2<> mPos) {
	for (auto rows : mGrid) {
		for (auto node : rows) {
			if (node->mPos.x == mPos.x && node->mPos.y == mPos.y)
				return node;
		}
	}

	return nullptr;
}

// Manhattan Distance
int CSearchAlgorithm::Heuristic(CTree::Node Start, CTree::Node end) {
	return abs(Start->mPos.x - end->mPos.x) + abs(Start->mPos.y - end->mPos.y);
}
