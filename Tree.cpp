#include "Tree.hpp"

#include <deque>
#include <set>
#include <map>
#include <iostream>

using namespace std;

CTree::CTree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid) : mWidth(dims.x), mHeight(dims.y), mGrid(_grid) {
	
}

CTree::Node CTree::SetNode(unsigned x, unsigned y, Vec2<> mPos, int mCost) {
	Node n = make_shared<SNode>(mPos, mCost);
	mGrid[y][x] = n;

	return n;
}

CTree::Node CTree::GetNode(unsigned x, unsigned y) {
	return mGrid[y][x];
}

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
