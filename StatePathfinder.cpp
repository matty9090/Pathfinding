#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

#include <cassert>
#include <fstream>

using namespace tle;

CStatePathfinder::CStatePathfinder(tle::I3DEngine *engine, CSettings &settings)
	: CState(engine, settings), mMap(*this), mTree(settings.getCurrentMap().mDims, mMap.mMapData)
{
	mNodeTypes[0] = make_pair("Wall", 0);
	mNodeTypes[1] = make_pair("Clear", 1);
	mNodeTypes[2] = make_pair("Wood", 2);
	mNodeTypes[3] = make_pair("Water", 3);
}

void CStatePathfinder::Init() {
	mPathNum = 0;
	mTimer   = 0.f;
	mBezier  = true;
	mUseDiag = false;
	mFoundA = true, mFoundB = true;

	for (auto key : mSettings.GetKeys())
		mKeyList += key.second.mDesc + "  /  ";

	mKeyList = mKeyList.substr(0, mKeyList.length() - 5);

	mpCam = mpEngine->CreateCamera(tle::kManual, 0.0f, 100.0f, -25.f);
	mpFont = mpEngine->LoadFont("res/Arial.ttf", 24U);

	LoadMaps();
	LoadModels();

	mScale = mSettings.GetMapScale();
	mOrigin = Vec3<>(mScale / 2.0f - (((float)mDims.x * mScale) / 2.0f), 12.0f, 10.0f);
	mAlg = AlgAStar;

	mMap.constructMap(mOrigin, mScale);
}

int CStatePathfinder::Run() {
	while (mpEngine->IsRunning()) {
		HandleInput();

		if (mpEngine->KeyHit(mSettings.GetKeyCode("Main Menu"))) {
			Cleanup();
			return CState::Menu;
		}

		mTimer -= mpEngine->Timer();

		if (mTimer < 0.0f) {
			mTimer = 0.07f;

			if (!mFoundA) {
				int r = mSearcherA->Step();

				if (r == CSearchAlgorithm::Found) {
					mFoundA = true;

					if (mBezier)
						DisplayPathBezier(mSearcherA->GetPath(), "Path_AS");
					else
						DisplayPathCatmullRom(mSearcherA->GetPath(), "Path_AS");

					WriteResults(mSearcherA->GetPath(), "AS", mSearcherA->GetNumSearches());

				} else
					DisplayPathSearch(mSearcherA->GetOpenList(), mSearcherA->GetClosedList(), "_AS");
			}

			if (!mFoundB) {
				int r = mSearcherB->Step();

				if (r == CSearchAlgorithm::Found) {
					mFoundB = true;

					if (mBezier)
						DisplayPathBezier(mSearcherB->GetPath(), "Path_BFS");
					else
						DisplayPathCatmullRom(mSearcherB->GetPath(), "Path_BFS");

					WriteResults(mSearcherB->GetPath(), "BFS", mSearcherB->GetNumSearches());
				} else
					DisplayPathSearch(mSearcherB->GetOpenList(), mSearcherB->GetClosedList(), "_BFS");
			}
		}

		DisplayGUI();
		mpEngine->DrawScene();
	}

	Cleanup();

	return CState::Exit;
}

void CStatePathfinder::LoadMaps() {
	CSettings::SMap curMap = mSettings.getCurrentMap();
	CMapLoader loader;
	pair<Vec2<>, Vec2<>> coords;

	mDims = curMap.mDims;
	loader.SetDims(mDims.x, mDims.y);

	auto m = loader.Load(mSettings.GetMapsFolder() + curMap.mMapFile);
	coords = loader.Coords(mSettings.GetMapsFolder() + curMap.mCoordsFile);

	mMap.mMapData.resize(mDims.y);

	for (unsigned y = 0; y < mDims.y; ++y)
		mMap.mMapData[y].resize(mDims.x);

	for (unsigned y = 0; y < mDims.y; ++y)
		for (unsigned x = 0; x < mDims.x; ++x)
			mMap.mMapData[y][x] = mTree.SetNode(x, y, Vec2<>(x, y), m[y][x]);

	mStart = mTree.FindNode(coords.first), mGoal = mTree.FindNode(coords.second);
}

void CStatePathfinder::LoadModels() {
	auto meshList = mSettings.GetModels();

	for (auto mesh : meshList) {
		mMeshes[mesh.first] = mpEngine->LoadMesh(mesh.second.mFile);

		if (mesh.second.mInst) {
			mModels[mesh.first] = mMeshes[mesh.first]->CreateModel(mesh.second.mPos.x, mesh.second.mPos.y, mesh.second.mPos.z);

			if(!mesh.second.mTex.empty())
				mModels[mesh.first]->SetSkin(mesh.second.mTex);
		}
	}

	assert(mMeshes.find("Clear") != mMeshes.end());
	assert(mMeshes.find("Wood")  != mMeshes.end());
	assert(mMeshes.find("Water") != mMeshes.end());
	assert(mMeshes.find("Wall")  != mMeshes.end());

	mpCam->RotateX(50.0f);
}

void CStatePathfinder::FreeMemory() {
	for (unsigned y = 0; y < mDims.y; ++y)
		mMap.mMapData[y].clear();

	mMap.mMapData.clear();
}

float CStatePathfinder::cspline(float p1, float p2, float p3, float p4, float t) {
	float a = -p1 + 3 * p2 - 3 * p3 + p4;
	float b = 2 * p1 - 5 * p2 + 4 * p3 - p4;
	float c = p3 - p1;
	float d = 2 * p2;

	return (a * t * t * t + b * t * t + c * t + d) / 2.0f;
}

float CStatePathfinder::bspline(float p1, float p2, float p3, float p4, float t) {
	return powf(1 - t, 3) * p1 + 3 * t * powf(1 - t, 2) * p2 + 3 * t * t * (1 - t) * p3 + t * t * t * p4;
}

float CStatePathfinder::lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

void CStatePathfinder::HandleInput() {
	if (mpEngine->KeyHit(Key_Escape))
		mpEngine->Stop();

	if (mpEngine->KeyHit(mSettings.GetKeyCode("Start"))) {
		ClearPathSearch();
		ClearPathLine();

		mFoundA = true, mFoundB = true;
		mPathNum = 0;
		mTimer = 0.0f;

		if (mAlg == AlgAStar) mFoundA = false;
		if (mAlg == AlgBfs)   mFoundB = false;
		if (mAlg == AlgBoth)  mFoundA = false, mFoundB = false;

		mSearcherA = make_shared<CAStar>(mTree);
		mSearcherB = make_shared<CBFS>(mTree);

		mSearcherA->UseDiagonals(mUseDiag);
		mSearcherA->Start(mStart, mGoal);

		mSearcherB->UseDiagonals(mUseDiag);
		mSearcherB->Start(mStart, mGoal);
	}

	if (mFoundA && mFoundB && mpEngine->KeyHit(mSettings.GetKeyCode("Clear"))) {
		ClearPathSearch();
		ClearPathLine();
	}

	if (mpEngine->KeyHit(mSettings.GetKeyCode("Switch Algorithm")))
		mAlg = (mAlg + 1) % 3;

	if (mpEngine->KeyHit(mSettings.GetKeyCode("Diagonals")))
		mUseDiag = !mUseDiag;

	if (mpEngine->KeyHit(mSettings.GetKeyCode("Curve Type")))
		mBezier = !mBezier;
}

void CStatePathfinder::DisplayGUI() {
	string algStr = (mAlg == AlgBfs) ? "Breadth-First" : ((mAlg == AlgAStar) ? "A*" : "BFS + A*");
	string diagStr = (mUseDiag ? "On" : "Off");
	string curvStr = (mBezier ? "Bezier" : "Catmul-Rom");

	mpFont->Draw(mKeyList, 10, 10, kWhite);
	mpFont->Draw("Diagonals: " + diagStr + "  /  " + "Curve: " + curvStr + "  /  " + "Algorithm: " + algStr, 10, 34, kLightGrey);
}

void CStatePathfinder::ClearPathSearch() {
	for (auto &i : mSearchPath)
		i->GetMesh()->RemoveModel(i);

	mSearchPath.clear();
}

void CStatePathfinder::ClearPathLine() {
	for (unsigned i = 0; i < mPath.size(); i++) {
		for (auto &node : mPath[i])
			node->GetMesh()->RemoveModel(node);

		mPath[i].clear();
	}

	mPath.clear();
}

void CStatePathfinder::WriteResults(std::vector<Vec2<>> path, string ext, int searches) {
	string mapFile = mSettings.getCurrentMap().mMapFile;
	ofstream file("Output_" + mapFile.substr(0, mapFile.find_last_of('.')) + "_" + ext + ".txt");

	for (auto coord : path)
		file << coord.toString() << endl;

	file << endl << "Number of linear searches: " << searches << endl;

	file.close();
}

void CStatePathfinder::DisplayPathSearch(std::set<CTree::Node> open, std::set<CTree::Node> closed, string id) {
	for (auto &node : open) {
		int x = node->mPos.x, y = node->mPos.y;

		mSearchPath.push_back(mMeshes["OpenList" + id]->CreateModel(x * mScale + mOrigin.x, mOrigin.y + 5.0f, y * mScale + mOrigin.z));
		mSearchPath.back()->SetSkin(mSettings.GetModels()["OpenList" + id].mTex);
		mSearchPath.back()->Scale(mSettings.GetModels()["OpenList" + id].mScale);
	}

	for (auto &node : closed) {
		int x = node->mPos.x, y = node->mPos.y;

		mSearchPath.push_back(mMeshes["ClosedList" + id]->CreateModel(x * mScale + mOrigin.x, mOrigin.y + 5.0f, y * mScale + mOrigin.z));
		mSearchPath.back()->SetSkin(mSettings.GetModels()["ClosedList" + id].mTex);
		mSearchPath.back()->Scale(mSettings.GetModels()["ClosedList" + id].mScale);
	}
}

void CStatePathfinder::DisplayPathBezier(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		mPath.resize(++mPathNum);
		
		int steps = 30;

		for (unsigned i = 0; i < p.size(); i++) {
			Vec2<> ps = (i > 0) ? p[i - 1] : p.front();
			Vec2<> pm = p[i];
			Vec2<> pf = (i < p.size() - 1) ? p[i + 1] : p.back();

			Vec2<float> dirA = Vec2<float>((float)pm.x, (float)pm.y) - Vec2<float>((float)ps.x, (float)ps.y);
			Vec2<float> dirB = Vec2<float>((float)pf.x, (float)pf.y) - Vec2<float>((float)pm.x, (float)pm.y);

			// Corner ahead
			if (ps.x != pf.x && ps.y != pf.y) {
				Vec2<float> p1 = Vec2<float>((float)ps.x, (float)ps.y) + dirA / 2.0f;
				Vec2<float> p2((float)pm.x, (float)pm.y);
				Vec2<float> p3 = p2 + dirB / 2.0f;
				Vec2<float> p4 = p3;

				for (int n = 0; n < steps; n++) {
					float cx = bspline(p1.x, p2.x, p3.x, p4.x, (float)n / (float)steps);
					float cy = bspline(p1.y, p2.y, p3.y, p4.y, (float)n / (float)steps);

					mPath[mPathNum - 1].push_back(mMeshes[id]->CreateModel(cx * mScale + mOrigin.x, mOrigin.y + 5.0f + (mPathNum), cy * mScale + mOrigin.z));
					mPath[mPathNum - 1].back()->SetSkin(mSettings.GetModels()[id].mTex);
					mPath[mPathNum - 1].back()->Scale(mSettings.GetModels()[id].mScale);
				}
			} else {
				for (int n = 0; n < steps; n++) {
					float cx = lerp(pm.x - dirA.x / 2.0f, pm.x + dirB.x / 2.0f, (float)n / (float)steps);
					float cy = lerp(pm.y - dirA.y / 2.0f, pm.y + dirB.y / 2.0f, (float)n / (float)steps);

					mPath[mPathNum - 1].push_back(mMeshes[id]->CreateModel(cx * mScale + mOrigin.x, mOrigin.y + 5.0f + (mPathNum), cy * mScale + mOrigin.z));
					mPath[mPathNum - 1].back()->SetSkin(mSettings.GetModels()[id].mTex);
					mPath[mPathNum - 1].back()->Scale(mSettings.GetModels()[id].mScale);
				}
			}
		}
	}
}

void CStatePathfinder::DisplayPathCatmullRom(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		mPath.resize(++mPathNum);

		int steps = 14;

		for (unsigned i = 0; i < p.size(); i++) {
			Vec2<> p1 = (i > 0) ? p[i - 1] : p[0];
			Vec2<> p2 = p[i];
			Vec2<> p3 = (i < p.size() - 1) ? p[i + 1] : p.back();
			Vec2<> p4 = (i < p.size() - 2) ? p[i + 2] : p.back();

			for (int i = 0; i < steps; i++) {
				float cx = cspline((float)p1.x, (float)p2.x, (float)p3.x, (float)p4.x, (float)i / (float)steps);
				float cy = cspline((float)p1.y, (float)p2.y, (float)p3.y, (float)p4.y, (float)i / (float)steps);

				mPath[mPathNum - 1].push_back(mMeshes[id]->CreateModel(cx * mScale + mOrigin.x, mOrigin.y + 5.0f + (mPathNum), cy * mScale + mOrigin.z));
				mPath[mPathNum - 1].back()->SetSkin(mSettings.GetModels()[id].mTex);
				mPath[mPathNum - 1].back()->Scale(mSettings.GetModels()[id].mScale);
			}
		}
	}
}

void CStatePathfinder::Cleanup() {
	ClearPathSearch();
	ClearPathLine();

	for (auto mesh : mMeshes)
		mpEngine->RemoveMesh(mesh.second);

	mKeyList = "";
	mpEngine->RemoveFont(mpFont);
}

void CStatePathfinder::SNodeMap::constructMap(Vec3<> origin, float scale) {
	mModels.resize(mParent.mDims.y);

	for (unsigned y = 0; y < mParent.mDims.y; ++y)
		mModels[y].resize(mParent.mDims.x);

	for (unsigned y = 0; y < mParent.mDims.y; ++y) {
		for (unsigned x = 0; x < mParent.mDims.y; ++x) {
			string type = mParent.mNodeTypes[mMapData[y][x]->mCost].first;
			Vec3<> v = translate(Vec2<>(x, y), origin, scale);

			mModels[y][x] = mParent.mMeshes[type]->CreateModel(v.x, v.y, v.z);
			mModels[y][x]->Scale(mParent.mSettings.GetModels()[type].mScale);

			if(!mParent.mSettings.GetModels()[type].mTex.empty())
				mModels[y][x]->SetSkin(mParent.mSettings.GetModels()[type].mTex);
		}
	}
}

Vec3<> CStatePathfinder::SNodeMap::translate(Vec2<> coord, Vec3<> &origin, float scale) {
	return Vec3<>(coord.x * scale + origin.x, origin.y, coord.y * scale + origin.z);
}
