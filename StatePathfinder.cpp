#include "StatePathfinder.hpp"
#include "MapLoader.hpp"

#include <cassert>
#include <fstream>

using namespace tle;

// Constructor initialises some member variables (lightweight enough so doesn't need to call init())
CStatePathfinder::CStatePathfinder(tle::I3DEngine *engine, CSettings &settings)
	: CState(engine, settings), mMap(*this), mTree(settings.GetMapSize(), mMap.mMapData)
{
	mNodeTypes[0] = make_pair("Wall", 0);
	mNodeTypes[1] = make_pair("Clear", 1);
	mNodeTypes[2] = make_pair("Wood", 2);
	mNodeTypes[3] = make_pair("Water", 3);
}

// Initialise path finding variables and scene
void CStatePathfinder::Init() {
	mPathNum = 0;
	mTimer   = 0.f;
	mBezier  = true;
	mUseDiag = false;
	mFoundA = true, mFoundB = true;

	// List of keys to be displayed on the UI to the user
	for (auto key : mSettings.GetKeys())
		mKeyList += key.second.mDesc + "  /  ";

	// Remove last '/' from the list
	mKeyList = mKeyList.substr(0, mKeyList.length() - 5);

	// Set up camera and load GUI font
	mpCam = mpEngine->CreateCamera(tle::kManual, 0.0f, 100.0f, -25.f);
	mpFont = mpEngine->LoadFont("res/Arial.ttf", 24U);

	// Load map and models
	LoadMaps();
	LoadModels();

	// Set up the map scale and origin so it's positioned properly
	mScale = mSettings.GetMapScale();
	mOrigin = Vec3<>(mScale / 2.0f - (((float)mDims.x * mScale) / 2.0f), 12.0f, 10.0f);
	mAlg = AlgAStar;

	// Use helper class to attach models to the node grid
	mMap.constructMap(mOrigin, mScale);
}

// Game loop
int CStatePathfinder::Run() {
	while (mpEngine->IsRunning()) {
		HandleInput();

		if (mpEngine->KeyHit(mSettings.GetKeyCode("Main Menu"))) {
			Cleanup();
			return CState::Menu;
		}

		mTimer -= mpEngine->Timer();

		// Steps through the algorithm one time every 0.07 seconds
		if (mTimer < 0.0f) {
			mTimer = 0.07f;

			// If A* not found goal step through
			if (!mFoundA) {
				int r = mSearcherA->Step();

				// If found goal, display the interpolated path, otherwise draw the open/closed list nodes
				if (r == CSearchAlgorithm::Found) {
					mFoundA = true;

					if (mBezier)
						DisplayPathBezier(mSearcherA->GetPath(), "Path_AS");
					else
						DisplayPathCatmullRom(mSearcherA->GetPath(), "Path_AS");

					// Write results to file
					WriteResults(mSearcherA->GetPath(), "AS", mSearcherA->GetNumSearches());
				} else
					DisplayPathSearch(mSearcherA->GetOpenList(), mSearcherA->GetClosedList(), "_AS");
			}

			// If BFS not found goal step through
			if (!mFoundB) {
				int r = mSearcherB->Step();

				// If found goal, display the interpolated path, otherwise draw the open/closed list nodes
				if (r == CSearchAlgorithm::Found) {
					mFoundB = true;

					if (mBezier)
						DisplayPathBezier(mSearcherB->GetPath(), "Path_BFS");
					else
						DisplayPathCatmullRom(mSearcherB->GetPath(), "Path_BFS");

					// Write results to file
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

// Load the map
void CStatePathfinder::LoadMaps() {
	CSettings::SMap curMap = mSettings.GetCurrentMap();
	CMapLoader loader;
	pair<Vec2<>, Vec2<>> coords;

	// Initialise dimensions
	mDims = curMap.mDims;
	loader.SetDims(mDims.x, mDims.y);

	// Use helper class to read in the map + coordinate files
	auto m = loader.Load(mSettings.GetMapsFolder() + curMap.mMapFile);
	coords = loader.Coords(mSettings.GetMapsFolder() + curMap.mCoordsFile);

	// Allocate memory for the map data
	mMap.mMapData.resize(mDims.y);

	for (unsigned y = 0; y < mDims.y; ++y)
		mMap.mMapData[y].resize(mDims.x);

	for (unsigned y = 0; y < mDims.y; ++y)
		for (unsigned x = 0; x < mDims.x; ++x)
			mMap.mMapData[y][x] = mTree.SetNode(x, y, Vec2<>(x, y), m[y][x]); // Set the nodes values read in from the map loader

	// Return a pointer to the start and end nodes by searching through the map
	mStart = mTree.FindNode(coords.first), mGoal = mTree.FindNode(coords.second);
}

// Create the models using the data from settings
void CStatePathfinder::LoadModels() {
	auto meshList = mSettings.GetModels();

	// Loop through all the meshes defined in settings
	for (auto mesh : meshList) {
		// Load the mesh
		mMeshes[mesh.first] = mpEngine->LoadMesh(mesh.second.mFile);

		// If it's a 'one-off' mesh aka only 1 instance needed (e.g. skybox), create a model in the scene
		if (mesh.second.mInst) {
			mModels[mesh.first] = mMeshes[mesh.first]->CreateModel(mesh.second.mPos.x, mesh.second.mPos.y, mesh.second.mPos.z);

			// Set the skin if not default
			if(!mesh.second.mTex.empty())
				mModels[mesh.first]->SetSkin(mesh.second.mTex);
		}
	}

	// Check the meshes exist
	assert(mMeshes.find("Clear") != mMeshes.end());
	assert(mMeshes.find("Wood")  != mMeshes.end());
	assert(mMeshes.find("Water") != mMeshes.end());
	assert(mMeshes.find("Wall")  != mMeshes.end());

	// Rotate the camera down for a top down view
	mpCam->RotateX(50.0f);
}

// Free up the map data memory
void CStatePathfinder::FreeMemory() {
	for (unsigned y = 0; y < mDims.y; ++y)
		mMap.mMapData[y].clear();

	mMap.mMapData.clear();
}

// Catmull-rom spline
float CStatePathfinder::cspline(float p1, float p2, float p3, float p4, float t) {
	float a = -p1 + 3 * p2 - 3 * p3 + p4;
	float b = 2 * p1 - 5 * p2 + 4 * p3 - p4;
	float c = p3 - p1;
	float d = 2 * p2;

	return (a * t * t * t + b * t * t + c * t + d) / 2.0f;
}

// Bezier spline
float CStatePathfinder::bspline(float p1, float p2, float p3, float p4, float t) {
	return powf(1 - t, 3) * p1 + 3 * t * powf(1 - t, 2) * p2 + 3 * t * t * (1 - t) * p3 + t * t * t * p4;
}

// Simple linear interpolation
float CStatePathfinder::lerp(float v0, float v1, float t) {
	return (1 - t) * v0 + t * v1;
}

// Handle keyboard inputs
void CStatePathfinder::HandleInput() {
	if (mpEngine->KeyHit(Key_Escape))
		mpEngine->Stop();

	// Key press to start the search
	if (mpEngine->KeyHit(mSettings.GetKeyCode("Start"))) {
		// Clear any previous searches
		ClearPathSearch();
		ClearPathLine();

		mSearcherA.reset();
		mSearcherB.reset();

		// (Re)initialise values for the search
		mFoundA = true, mFoundB = true;
		mPathNum = 0;
		mTimer = 0.0f;

		// If using only 1 algorithm, leave the other one found to true as to not run the algorithm
		if (mAlg == AlgAStar) mFoundA = false;
		if (mAlg == AlgBfs)   mFoundB = false;
		if (mAlg == AlgBoth)  mFoundA = false, mFoundB = false;

		// Create new objects for the searchers
		mSearcherA = make_shared<CAStar>(mTree);
		mSearcherB = make_shared<CBFS>(mTree);

		// Initialise values and start the search
		mSearcherA->UseDiagonals(mUseDiag);
		mSearcherA->Start(mStart, mGoal);

		mSearcherB->UseDiagonals(mUseDiag);
		mSearcherB->Start(mStart, mGoal);
	}

	// Button to clear the searches
	if (mFoundA && mFoundB && mpEngine->KeyHit(mSettings.GetKeyCode("Clear"))) {
		ClearPathSearch();
		ClearPathLine();
	}

	// Toggle between algorithms on key press
	if (mpEngine->KeyHit(mSettings.GetKeyCode("Switch Algorithm")))
		mAlg = (mAlg + 1) % 3;

	// Button to toggle diagonals on/off
	if (mpEngine->KeyHit(mSettings.GetKeyCode("Diagonals")))
		mUseDiag = !mUseDiag;

	// Button to toggle the curve type between bezier/catmull-rom
	if (mpEngine->KeyHit(mSettings.GetKeyCode("Curve Type")))
		mBezier = !mBezier;
}

// Draw the UI elements
void CStatePathfinder::DisplayGUI() {
	string algStr = (mAlg == AlgBfs) ? "Breadth-First" : ((mAlg == AlgAStar) ? "A*" : "BFS + A*");
	string diagStr = (mUseDiag ? "On" : "Off");
	string curvStr = (mBezier ? "Bezier" : "Catmul-Rom");

	mpFont->Draw(mKeyList, 10, 10, kWhite);
	mpFont->Draw("Diagonals: " + diagStr + "  /  " + "Curve: " + curvStr + "  /  " + "Algorithm: " + algStr, 10, 34, kLightGrey);
}

// Clear all the models associated with the path line
void CStatePathfinder::ClearPathSearch() {
	for (auto &i : mSearchPath)
		i->GetMesh()->RemoveModel(i);

	mSearchPath.clear();
}

// Clear all the models associated with visualising the open and closed list
void CStatePathfinder::ClearPathLine() {
	for (unsigned i = 0; i < mPath.size(); i++) {
		for (auto &node : mPath[i])
			node->GetMesh()->RemoveModel(node);

		mPath[i].clear();
	}

	mPath.clear();
}

// Write the results to the file
void CStatePathfinder::WriteResults(std::vector<Vec2<>> path, string ext, int searches) {
	string mapFile = mSettings.GetCurrentMap().mMapFile;
	ofstream file("Output_" + mapFile.substr(0, mapFile.find_last_of('.')) + "_" + ext + ".txt");

	for (auto coord : path)
		file << coord.toString() << endl;

	file << endl << "Number of linear searches: " << searches << endl;

	file.close();
}

// Draw the open and closed list using models
void CStatePathfinder::DisplayPathSearch(std::set<CTree::Node> open, std::set<CTree::Node> closed, string id) {
	for (auto &node : open) {
		int x = node->mPos.x, y = node->mPos.y;

		// Create model and display at correct position using the scale and origin of the map
		mSearchPath.push_back(mMeshes["OpenList" + id]->CreateModel(x * mScale + mOrigin.x, mOrigin.y + 5.0f, y * mScale + mOrigin.z));
		mSearchPath.back()->SetSkin(mSettings.GetModels()["OpenList" + id].mTex);
		mSearchPath.back()->Scale(mSettings.GetModels()["OpenList" + id].mScale);
	}

	for (auto &node : closed) {
		int x = node->mPos.x, y = node->mPos.y;

		// Create model and display at correct position using the scale and origin of the map
		mSearchPath.push_back(mMeshes["ClosedList" + id]->CreateModel(x * mScale + mOrigin.x, mOrigin.y + 5.0f, y * mScale + mOrigin.z));
		mSearchPath.back()->SetSkin(mSettings.GetModels()["ClosedList" + id].mTex);
		mSearchPath.back()->Scale(mSettings.GetModels()["ClosedList" + id].mScale);
	}
}

// Draw a path using bezier for the corners
void CStatePathfinder::DisplayPathBezier(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		// mPath is a vector to support multiple paths drawing simultaneously, expand for new path
		mPath.resize(++mPathNum);
		
		// How many models to draw in 1 line segment
		int steps = 30;

		// Loop through every path node
		for (unsigned i = 0; i < p.size(); i++) {
			// Ge start, midpoint and final node coordinates (the 3 nodes associated with a corner)
			Vec2<> ps = (i > 0) ? p[i - 1] : p.front();
			Vec2<> pm = p[i];
			Vec2<> pf = (i < p.size() - 1) ? p[i + 1] : p.back();

			// Calculate the 2 directions involved between the 3 nodes
			Vec2<float> dirA = Vec2<float>((float)pm.x, (float)pm.y) - Vec2<float>((float)ps.x, (float)ps.y);
			Vec2<float> dirB = Vec2<float>((float)pf.x, (float)pf.y) - Vec2<float>((float)pm.x, (float)pm.y);

			// Corner ahead
			if (ps.x != pf.x && ps.y != pf.y) {
				// Pick the 4 points to use for the bezier curve (using the 3 nodes coordinates above)
				Vec2<float> p1 = Vec2<float>((float)ps.x, (float)ps.y) + dirA / 2.0f;
				Vec2<float> p2((float)pm.x, (float)pm.y);
				Vec2<float> p3 = p2 + dirB / 2.0f;
				Vec2<float> p4 = p3;

				// Draw the curve, using the bezier spline calculations
				for (int n = 0; n < steps; n++) {
					float cx = bspline(p1.x, p2.x, p3.x, p4.x, (float)n / (float)steps);
					float cy = bspline(p1.y, p2.y, p3.y, p4.y, (float)n / (float)steps);

					// Create model and display at correct position using the scale and origin of the map
					mPath[mPathNum - 1].push_back(mMeshes[id]->CreateModel(cx * mScale + mOrigin.x, mOrigin.y + 5.0f + (mPathNum), cy * mScale + mOrigin.z));
					mPath[mPathNum - 1].back()->SetSkin(mSettings.GetModels()[id].mTex);
					mPath[mPathNum - 1].back()->Scale(mSettings.GetModels()[id].mScale);
				}
			} else {
				// Just draw a linear line since no corner (straight)
				for (int n = 0; n < steps; n++) {
					float cx = lerp(pm.x - dirA.x / 2.0f, pm.x + dirB.x / 2.0f, (float)n / (float)steps);
					float cy = lerp(pm.y - dirA.y / 2.0f, pm.y + dirB.y / 2.0f, (float)n / (float)steps);

					// Create model and display at correct position using the scale and origin of the map
					mPath[mPathNum - 1].push_back(mMeshes[id]->CreateModel(cx * mScale + mOrigin.x, mOrigin.y + 5.0f + (mPathNum), cy * mScale + mOrigin.z));
					mPath[mPathNum - 1].back()->SetSkin(mSettings.GetModels()[id].mTex);
					mPath[mPathNum - 1].back()->Scale(mSettings.GetModels()[id].mScale);
				}
			}
		}
	}
}

// Draw the path using Catmull-Rom splines
void CStatePathfinder::DisplayPathCatmullRom(std::vector<Vec2<>> p, string id) {
	if (!p.empty()) {
		// mPath is a vector to support multiple paths drawing simultaneously, expand for new path
		mPath.resize(++mPathNum);

		// How many models to draw in 1 line segment
		int steps = 14;

		// Loop through every path node
		for (unsigned i = 0; i < p.size(); i++) {
			// Pick the 4 points around the current node to create the spline with
			Vec2<> p1 = (i > 0) ? p[i - 1] : p[0];
			Vec2<> p2 = p[i];
			Vec2<> p3 = (i < p.size() - 1) ? p[i + 1] : p.back();
			Vec2<> p4 = (i < p.size() - 2) ? p[i + 2] : p.back();

			// Draw the curve, using the Catmull-Rom spline calculations
			for (int i = 0; i < steps; i++) {
				float cx = cspline((float)p1.x, (float)p2.x, (float)p3.x, (float)p4.x, (float)i / (float)steps);
				float cy = cspline((float)p1.y, (float)p2.y, (float)p3.y, (float)p4.y, (float)i / (float)steps);

				// Create model and display at correct position using the scale and origin of the map
				mPath[mPathNum - 1].push_back(mMeshes[id]->CreateModel(cx * mScale + mOrigin.x, mOrigin.y + 5.0f + (mPathNum), cy * mScale + mOrigin.z));
				mPath[mPathNum - 1].back()->SetSkin(mSettings.GetModels()[id].mTex);
				mPath[mPathNum - 1].back()->Scale(mSettings.GetModels()[id].mScale);
			}
		}
	}
}

// Clean up meshes and free memory
void CStatePathfinder::Cleanup() {
	ClearPathSearch();
	ClearPathLine();

	for (auto mesh : mMeshes)
		mpEngine->RemoveMesh(mesh.second);

	mKeyList = "";
	mpEngine->RemoveFont(mpFont);
}

// Create all the models using the node map data
void CStatePathfinder::SNodeMap::constructMap(Vec3<> origin, float scale) {
	// Allocate memory for the container to store the models
	mModels.resize(mParent.mDims.y);

	for (unsigned y = 0; y < mParent.mDims.y; ++y)
		mModels[y].resize(mParent.mDims.x);

	// Loop through rows and columns of map
	for (unsigned y = 0; y < mParent.mDims.y; ++y) {
		for (unsigned x = 0; x < mParent.mDims.y; ++x) {
			// Get the type of node (Wood, Water etc...)
			string type = mParent.mNodeTypes[mMapData[y][x]->mCost].first;

			// Get the world position using the origin and scale of the map
			Vec3<> v = translate(Vec2<>(x, y), origin, scale);

			// Create the model at the position
			mModels[y][x] = mParent.mMeshes[type]->CreateModel(v.x, v.y, v.z);
			
			// Scale it using the settings data
			mModels[y][x]->Scale(mParent.mSettings.GetModels()[type].mScale);

			// Set the skin if it's not the default one
			if(!mParent.mSettings.GetModels()[type].mTex.empty())
				mModels[y][x]->SetSkin(mParent.mSettings.GetModels()[type].mTex);
		}
	}
}

// Helper function to translate a coordinate using the origin and scale of the world
Vec3<> CStatePathfinder::SNodeMap::translate(Vec2<> coord, Vec3<> &origin, float scale) {
	return Vec3<>(coord.x * scale + origin.x, origin.y, coord.y * scale + origin.z);
}
