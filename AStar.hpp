#pragma once

#include "Tree.hpp"

class CAStar : public CSearchAlgorithm {
	public:
		CAStar(CTree &tree);

		void Start(CTree::Node Start, CTree::Node goal);
		int Step();

	private:
		std::map<CTree::Node, CTree::Node> mData;

		bool mGoalFound;

		void mConstructPath();
};