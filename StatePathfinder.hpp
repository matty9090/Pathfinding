#pragma once

#include <TL-Engine.h>
#include <vector>
#include <map>
#include <list>
#include <memory>

#include "Settings.hpp"
#include "State.hpp"
#include "Tree.hpp"
#include "BFS.hpp"
#include "AStar.hpp"

/*
	State to handle the pathfinding
*/

class CStatePathfinder : public CState {
	public:
		CStatePathfinder(tle::I3DEngine *engine, CSettings &settings);

		void Init();
		int Run();

		void LoadMaps();
		void LoadModels();
		void FreeMemory();

	private:
		CTree mTree;
		CTree::Node mStart, mGoal;
		Vec2<size_t> mDims;

		bool mFoundA, mFoundB, mUseDiag, mBezier;
		int mPathNum;
		float mScale, mTimer;
		Vec3<> mOrigin;

		enum EAlgorithms { AlgBfs, AlgAStar, AlgBoth };
		int mAlg;

		tle::ICamera *mpCam;
		tle::IFont *mpFont;
		std::string mKeyList;

		std::shared_ptr<CSearchAlgorithm> mSearcherA, mSearcherB;
		std::vector<tle::IModel*> mSearchPath;
		std::vector<std::vector<tle::IModel*>> mPath;

		std::map<std::string, tle::IMesh*> mMeshes;
		std::map<std::string, tle::IModel*> mModels;
		std::map<int, std::pair<std::string, int>> mNodeTypes;

		// Encapsulate map data (associates models with the node map)
		struct SNodeMap {
			std::vector<std::vector<CTree::Node>> mMapData;
			std::vector<std::vector<tle::IModel*>> mModels;

			CStatePathfinder &mParent;

			SNodeMap(CStatePathfinder &parent) : mParent(parent) {}

			void constructMap(Vec3<> origin, float scale);
			inline Vec3<> translate(Vec2<> coord, Vec3<> &origin, float scale);
		} mMap;

		float cspline(float p1, float p2, float p3, float p4, float t);
		float bspline(float p1, float p2, float p3, float p4, float t);
		float lerp(float v0, float v1, float t);

		void HandleInput();
		void DisplayGUI();
		void ClearPathSearch();
		void ClearPathLine();
		void WriteResults(std::vector<Vec2<>> path, string ext, int searches);
		void DisplayPathSearch(std::set<CTree::Node> open, std::set<CTree::Node> closed, string id);
		void DisplayPathBezier(std::vector<Vec2<>> path, std::string id);
		void DisplayPathCatmullRom(std::vector<Vec2<>> path, std::string id);
		void Cleanup();
};