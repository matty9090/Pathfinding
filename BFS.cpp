#include "BFS.hpp"

#include <iostream>

using namespace std;

/* Initialise using a node tree from a map */
CBFS::CBFS(CTree &tree) : CSearchAlgorithm(tree) {

}

/* Set up values */
void CBFS::Start(CTree::Node start, CTree::Node goal) {
	mStartNode = start;
	mGoalNode  = goal;

	mNumSearches = 0;
	mGoalFound   = false;

	// Add start node to open list
	mOpenList.insert(mStartNode);

	cout << "Starting at (" << mStartNode->mPos.x << ", " << mStartNode->mPos.y << ")\n";
	cout << "Finding (" << mGoalNode->mPos.x << ", " << mGoalNode->mPos.y << ")\n\n";
}

/* One step/loop iteration of the algorithm */
int CBFS::Step() {
	// Loop through open list
	if (!mOpenList.empty()) {
		// Get the first node off the open list
		CTree::Node current = *mOpenList.begin();

		// Move the current node from the open list to the closed list
		mOpenList.erase(current);
		mClosedList.insert(current);

		// If this node is the goal then we found a path
		if (current == mGoalNode) {
			mGoalFound = true;
			mConstructPath();
			return Found;
		}

		// Find all the neighbours of the current node
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

		// Loop through all the neighbours and add them to the open list if they aren't already on there
		for (auto node : next) {
			// Discard nodes which are in the closed list already, aren't walkable or nodes which don't exist (out of bounds)
			if (node == nullptr || node->mCost <= 0 || mClosedList.find(node) != mClosedList.end())
				continue;

			if (mOpenList.find(node) == mOpenList.end()) {
				mData[node] = current;
				mOpenList.insert(node);
			}
		}
	} else
		return Failed;
	
	return Searching;
}

/* Construct the path by following the nodes backwards(if the goal is found) */
void CBFS::mConstructPath() {
	if (mGoalFound) {
		CTree::Node n = mGoalNode;
		mPath.push_back(n->mPos);

		while (n = mData[n]) {
			mPath.push_back(n->mPos);
			cout << n->mPos.toString() << endl;
		}

		// Reverse this path to get it forwards
		reverse(mPath.begin(), mPath.end());
	} else
		cout << "Could not find path\n\n";
}
