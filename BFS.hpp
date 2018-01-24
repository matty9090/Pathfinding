/// Author: Matthew Lowe
/// Copyright (c) 2018

#pragma once

#include "Tree.hpp"

/*
	Breadth-First search implementation
*/

class CBFS : public CSearchAlgorithm
{
	public:
		CBFS(CTree &tree);

		void Start(CTree::Node Start, CTree::Node goal);
		int Step();

	private:
		std::map<CTree::Node, CTree::Node> mData;

		bool mGoalFound;

		void mConstructPath();
};