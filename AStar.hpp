#pragma once

#include "Tree.hpp"

/*
	A* search algorithm implementation
*/

class CAStar : public CSearchAlgorithm
{
	public:
		CAStar(CTree &tree);

		void Start(CTree::Node start, CTree::Node goal);
		int Step();

	private:
		std::map<CTree::Node, CTree::Node> mData;

		bool mGoalFound;

		void mConstructPath();
};