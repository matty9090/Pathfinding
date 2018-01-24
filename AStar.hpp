/// Author: Matthew Lowe
/// Copyright (c) 2018

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