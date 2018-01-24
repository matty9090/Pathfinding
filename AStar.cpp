/// Author: Matthew Lowe
/// Copyright (c) 2018

#include "AStar.hpp"

#include <iostream>

using namespace std;

/* Initialise using a node tree from a map */
CAStar::CAStar(CTree &tree) : CSearchAlgorithm(tree)
{

}

/* Set up values */
void CAStar::Start(CTree::Node start, CTree::Node goal)
{
	mStartNode = start;
	mGoalNode = goal;

	// Count the number of linear searches
	mNumSearches = 0;

	// Scores initialised to a high value
	for (unsigned y = 0; y < mTree.GetGridSize().y; y++)
	{
		for (unsigned x = 0; x < mTree.GetGridSize().x; x++)
		{
			mTree.GetNode(x, y)->mScore = 10000;
			mTree.GetNode(x, y)->mEstimate = 10000;
		}
	}

	// Set start node values
	mStartNode->mScore = 0;
	mStartNode->mEstimate = Heuristic(mStartNode, mGoalNode);

	// Add start node to open list
	mOpenList.insert(mStartNode);

	mGoalFound = false;

	cout << "Starting at (" << mStartNode->mPos.x << ", " << mStartNode->mPos.y << ")\n";
	cout << "Finding (" << mGoalNode->mPos.x << ", " << mGoalNode->mPos.y << ")\n\n";
}

/* One step/loop iteration of the algorithm */
int CAStar::Step()
{
	// Loop through open list
	if (!mOpenList.empty())
	{
		mNumSearches++;

		// Find the next node to process by choosing the lowest estimated score
		CTree::Node current;
		int minScore = 10000;

		for (auto n : mOpenList)
			if (n->mEstimate < minScore)
				minScore = n->mEstimate, current = n;

		// If this node is the goal then we found a path
		if (current == mGoalNode)
		{
			mGoalFound = true;
			mConstructPath();
			return Found;
		}

		// Move the current node from the open list to the closed list
		mOpenList.erase(current);
		mClosedList.insert(current);

		// Find all the neighbours of the current node
		std::vector<CTree::Node> next;

		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x + 1, current->mPos.y)));
		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x - 1, current->mPos.y)));
		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x, current->mPos.y + 1)));
		next.push_back(mTree.FindNode(Vec2<>(current->mPos.x, current->mPos.y - 1)));
		
		if (mUseDiag)
		{
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x + 1, current->mPos.y + 1)));
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x - 1, current->mPos.y - 1)));
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x - 1, current->mPos.y + 1)));
			next.push_back(mTree.FindNode(Vec2<>(current->mPos.x + 1, current->mPos.y - 1)));
		}

		// Loop through all the neighbours
		for (auto node : next)
		{
			// Discard nodes which are in the closed list already, aren't walkable or nodes which don't exist (out of bounds)
			if (node == nullptr || node->mCost <= 0 || mClosedList.find(node) != mClosedList.end())
				continue;

			// If node isn't in the open list, add it
			if (mOpenList.find(node) == mOpenList.end())
				mOpenList.insert(node);

			// Calculate the cost of the path to this node
			int nScore = current->mScore + current->mCost;

			// If the path is worse, discard it
			if (nScore >= node->mScore)
				continue;

			// The path is better, save it and work out an estimated score using a heuristic
			mData[node] = current;
			node->mScore = nScore;
			node->mEstimate = nScore + Heuristic(node, mGoalNode);
		}
	} else
		return Failed;
	
	return Searching;
}

/* Construct the path by following the nodes backwards(if the goal is found) */
void CAStar::mConstructPath()
{
	if (mGoalFound)
	{
		CTree::Node n = mGoalNode;
		mPath.push_back(n->mPos);

		while (n = mData[n])
			mPath.push_back(n->mPos);

		// Reverse this path to get it forwards
		reverse(mPath.begin(), mPath.end());
	} else
		cout << "Could not find path\n\n";
}
