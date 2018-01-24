#pragma once

#include <memory>
#include <vector>
#include <list>
#include <set>
#include <map>

#include "Vec3.hpp"

struct _Node {
	Vec2<> mPos;
	int mCost;
	int mScore;
	int mEstimate;

	_Node(Vec2<> p, int c) : mPos(p), mCost(c) {};
};

class CTree {
	public:
		typedef std::shared_ptr<_Node> Node;

		CTree(Vec2<size_t> dims, std::vector<std::vector<Node>> &_grid);

		Node SetNode(unsigned x, unsigned y, Vec2<> pos, int cost = 1);
		Node GetNode(unsigned x, unsigned y);
		Node FindNode(Vec2<> pos);

		Vec2<size_t> GetGridSize() { return Vec2<size_t>(mWidth, mHeight); }

	private:
		std::vector<std::vector<Node>> &mGrid;

		size_t mWidth, mHeight;
};

class CSearchAlgorithm {
	public:
		enum EStatus { Found, Searching, Failed };

		CSearchAlgorithm(CTree &t, bool diag = false) : mTree(t), mUseDiag(diag) {}

		virtual void Start(CTree::Node start, CTree::Node goal) = 0;
		virtual int Step() = 0;
		
		std::vector<Vec2<>> GetPath() { return mPath; }

		int GetNumSearches() { return mNumSearches; }
		void UseDiagonals(bool diag) { mUseDiag = diag; }

		std::set<CTree::Node> GetOpenList() { return mOpenList; }
		std::set<CTree::Node> GetClosedList() { return mClosedList; }

	protected:
		CTree &mTree;
		CTree::Node mStartNode, mGoalNode;

		std::vector<Vec2<>> mPath;
		std::set<CTree::Node> mOpenList;
		std::set<CTree::Node> mClosedList;

		int mNumSearches;
		bool mUseDiag;

		int Heuristic(CTree::Node start, CTree::Node end);
};