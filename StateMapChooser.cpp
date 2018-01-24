#include "StateMapChooser.hpp"
#include "MapLoader.hpp"

using namespace tle;

CStateMapChooser::CStateMapChooser(tle::I3DEngine *engine, CSettings &settings) : CState(engine, settings) {
	
}

void CStateMapChooser::Init() {
	// Load UI font and sprites
	mpFont			= mpEngine->LoadFont("res/HoboStd.otf", mSettings.GetMenuFontSize());
	mpSpriteLogo	= mpEngine->CreateSprite("res/logo.png", mSettings.GetLogoPos().x, mSettings.GetLogoPos().y);
	mpSprBg			= mpEngine->CreateSprite("res/bg.jpg");

	int count = 0;

	// Loop through maps and create menu items out of them
	for (auto map : mSettings.GetMaps()) {
		mItems[count] = { "Map " + to_string(count + 1), count };
		++count;
	}

	mSelected = 0;
	
	int i = 0;

	// Align the menu items
	for (auto &item : mItems) {
		item.second.mPos = Vec2<>(mSettings.GetMenuPos().x, i * mSettings.GetMenuSpacing() + mSettings.GetMenuPos().y);
		++i;
	}

	mStateChange = false;
	mReturnState = CState::Exit;
}

// Main loop
int CStateMapChooser::Run() {
	while (mpEngine->IsRunning() && !mStateChange) {
		if (mpEngine->KeyHit(Key_Escape)) {
			mpEngine->Stop();
			break;
		}

		// Rotate between menu items (wrapped)
		if (mpEngine->KeyHit(Key_Down)) mSelected = (mSelected + 1) % mItems.size();
		if (mpEngine->KeyHit(Key_Up)) mSelected = (mSelected - 1) % mItems.size();

		// Select the map and go to the pathfinding state
		if (mpEngine->KeyHit(Key_Return)) {
			mSettings.SetMap(mItems[mSelected].mTxt); // Save selected map to settings
			mReturnState = CState::Pathfinder;
			mStateChange = true;
		}

		// Draw each menu item
		for (auto &item : mItems)
			mpFont->Draw(item.second.mTxt, item.second.mPos.x, item.second.mPos.y, (item.first == mSelected) ? kBlue : kBlack, kCentre, kVCentre);

		mpEngine->DrawScene();
	}

	// Remove the sprites from the scene
	mpEngine->RemoveFont(mpFont);
	mpEngine->RemoveSprite(mpSpriteLogo);
	mpEngine->RemoveSprite(mpSprBg);

	return mReturnState;
}