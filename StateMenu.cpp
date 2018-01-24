#include "StateMenu.hpp"

using namespace tle;

CStateMenu::CStateMenu(tle::I3DEngine *engine, CSettings &settings) : CState(engine, settings) {
	
}

void CStateMenu::Init() {
	// Load UI font and sprites
	mpFont		= mpEngine->LoadFont("res/HoboStd.otf", 46U);
	mpSprLogo	= mpEngine->CreateSprite("res/logo.png", 314.0f, 76.0f);
	mpSprBg		= mpEngine->CreateSprite("res/bg.jpg");

	// Create menu items
	mItems[0] = { "Choose map", &CStateMenu::ActionSelect };
	mItems[1] = { "Exit", &CStateMenu::ActionExit };

	mSelected = 0;
	
	int i = 0;

	// Align the menu items
	for (auto &item : mItems) {
		item.second.mPos = Vec2<>(512, i * 56 + 380);
		++i;
	}

	mStateChange = false;
	mReturnState = CState::Exit;
}

// Main loop
int CStateMenu::Run() {
	while (mpEngine->IsRunning() && !mStateChange) {
		if (mpEngine->KeyHit(Key_Escape)) {
			mpEngine->Stop();
			break;
		}

		// Rotate between menu items (wrapped)
		if (mpEngine->KeyHit(Key_Down)) mSelected = (mSelected + 1) % mItems.size();
		if (mpEngine->KeyHit(Key_Up)) mSelected = (mSelected - 1) % mItems.size();
		if (mpEngine->KeyHit(Key_Return)) ((*this).*mItems[mSelected].mAction)();

		// Draw each menu item
		for (auto &item : mItems)
			mpFont->Draw(item.second.mTxt, item.second.mPos.x, item.second.mPos.y, (item.first == mSelected) ? kBlue : kBlack, kCentre, kVCentre);

		mpEngine->DrawScene();
	}

	// Remove the sprites from the scene
	mpEngine->RemoveFont(mpFont);
	mpEngine->RemoveSprite(mpSprLogo);
	mpEngine->RemoveSprite(mpSprBg);

	return mReturnState;
}

// Select map option
void CStateMenu::ActionSelect() {
	mStateChange = true;
	mReturnState = CState::MapChooser;
}

// Exit option
void CStateMenu::ActionExit() {
	mStateChange = true;
	mReturnState = CState::Exit;
}