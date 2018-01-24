#include "App.hpp"

#include "StateMenu.hpp"
#include "StateMapChooser.hpp"
#include "StatePathfinder.hpp"

using namespace tle;

CApp::CApp(CSettings &s) : mSettings(s) {
	mEng = New3DEngine(kTLX);

	mEng->StartWindowed(mSettings.GetScreenW(), mSettings.GetScreenH());
	mEng->SetWindowCaption("Pathfinding");
	mEng->AddMediaFolder("./res");

	mState = CState::Menu;

	mStates.push_back(make_unique<CStateMenu>(mEng, mSettings));
	mStates.push_back(make_unique<CStateMapChooser>(mEng, mSettings));
	mStates.push_back(make_unique<CStatePathfinder>(mEng, mSettings));
}

CApp::~CApp() {
	mEng->Delete();
}

void CApp::run() {
	do {
		mStates[mState]->Init();
		mState = mStates[mState]->Run();
	} while (mState != CState::Exit);
}
