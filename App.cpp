#include "App.hpp"

#include "StateMenu.hpp"
#include "StateMapChooser.hpp"
#include "StatePathfinder.hpp"

using namespace tle;

CApp::CApp(CSettings &s) : mSettings(s)
{
	// Init TL-Engine
	mEng = New3DEngine(kTLX);

	// Initialise window parameters
	mEng->StartWindowed(mSettings.GetScreenW(), mSettings.GetScreenH());
	mEng->SetWindowCaption("Pathfinding");
	mEng->AddMediaFolder("./res");

	// Starting state
	mState = CState::Menu;

	// Initialise list of states
	mStates.push_back(make_unique<CStateMenu>(mEng, mSettings));
	mStates.push_back(make_unique<CStateMapChooser>(mEng, mSettings));
	mStates.push_back(make_unique<CStatePathfinder>(mEng, mSettings));
}

/* Cleanup */
CApp::~CApp()
{
	mEng->Delete();
}

/* Infinite state loop, exits program when state is exit */
void CApp::run()
{
	do
	{
		// Initialise the state first
		mStates[mState]->Init();

		// Main loop for that state
		mState = mStates[mState]->Run();
	} while (mState != CState::Exit);
}
