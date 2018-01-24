#include "BFS.hpp"

#include <iostream>

using namespace std;

CBFS::CBFS(CTree &tree) : CSearchAlgorithm(tree) {

}

void CBFS::Start(CTree::Node _start, CTree::Node _goal) {
	mStartNode = _start;
	mGoalNode = _goal;

	mNumSearches = 0;

	mGoalFound = false;

	mOpenList.insert(mStartNode);

	cout << "Starting at (" << mStartNode->mPos.x << ", " << mStartNode->mPos.y << ")\n";
	cout << "Finding (" << mGoalNode->mPos.x << ", " << mGoalNode->mPos.y << ")\n\n";
}

int CBFS::Step() {
	if (!mOpenList.empty()) {
		mNumSearches++;

		CTree::Node current = *mOpenList.begin();
		mOpenList.erase(current);

		if (current == mGoalNode) {
			mGoalFound = true;
			mConstructPath();
			return Found;
		}

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

			if (mOpenList.find(node) == mOpenList.end()) {
				mData[node] = current;
				mOpenList.insert(node);
			}
		}

		mClosedList.insert(current);
	} else
		return Failed;
	
	return Searching;
}

void CBFS::mConstructPath() {
	if (mGoalFound) {
		CTree::Node n = mGoalNode;
		mPath.push_back(n->mPos);

		while (n = mData[n]) {
			mPath.push_back(n->mPos);
			cout << n->mPos.toString() << endl;
		}

		reverse(mPath.begin(), mPath.end());
	} else
		cout << "Could not find path\n\n";
}
