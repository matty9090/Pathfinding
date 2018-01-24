#include "AStar.hpp"

#include <iostream>

using namespace std;

CAStar::CAStar(CTree &tree) : CSearchAlgorithm(tree) {

}

void CAStar::Start(CTree::Node _start, CTree::Node _goal) {
	mStartNode = _start;
	mGoalNode = _goal;

	mNumSearches = 0;

	for (unsigned y = 0; y < mTree.GetGridSize().y; y++) {
		for (unsigned x = 0; x < mTree.GetGridSize().x; x++) {
			mTree.GetNode(x, y)->mScore = 10000;
			mTree.GetNode(x, y)->mEstimate = 10000;
		}
	}

	mStartNode->mScore = 0;
	mStartNode->mEstimate = Heuristic(mStartNode, mGoalNode);

	mOpenList.insert(mStartNode);

	mGoalFound = false;

	cout << "Starting at (" << mStartNode->mPos.x << ", " << mStartNode->mPos.y << ")\n";
	cout << "Finding (" << mGoalNode->mPos.x << ", " << mGoalNode->mPos.y << ")\n\n";
}

int CAStar::Step() {
	if (!mOpenList.empty()) {
		mNumSearches++;

		CTree::Node current;
		int minScore = 10000;

		for (auto n : mOpenList)
			if (n->mEstimate < minScore)
				minScore = n->mEstimate, current = n;

		if (current == mGoalNode) {
			mGoalFound = true;
			mConstructPath();
			return Found;
		}

		mOpenList.erase(current);
		mClosedList.insert(current);

		std::vector<CTree::Node> next;

		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x + 1, current->mPos.y)));
		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x - 1, current->mPos.y)));
		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x, current->mPos.y + 1)));
		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x, current->mPos.y - 1)));
		
		if (mUseDiag) {
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x + 1, current->mPos.y + 1)));
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x - 1, current->mPos.y - 1)));
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x - 1, current->mPos.y + 1)));
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x + 1, current->mPos.y - 1)));
		}

		for (auto node : next) {
			if (node == nullptr || node->mCost <= 0 || mClosedList.find(node) != mClosedList.end())
				continue;

			if (mOpenList.find(node) == mOpenList.end())
				mOpenList.insert(node);

			int nScore = current->mScore + current->mCost;

			if (nScore >= node->mScore)
				continue;

			mData[node] = current;
			node->mScore = nScore;
			node->mEstimate = nScore + Heuristic(node, mGoalNode);
		}
	} else
		return Failed;
	
	return Searching;
}

void CAStar::mConstructPath() {
	if (mGoalFound) {
		CTree::Node n = mGoalNode;
		mPath.push_back(n->mPos);

		while (n = mData[n])
			mPath.push_back(n->mPos);

		reverse(mPath.begin(), mPath.end());
	} else
		cout << "Could not find path\n\n";
}
