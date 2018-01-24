#include "StateMapChooser.hpp"
#include "MapLoader.hpp"

using namespace tle;

CStateMapChooser::CStateMapChooser(tle::I3DEngine *engine, CSettings &settings) : CState(engine, settings) {
	
}

void CStateMapChooser::Init() {
	mpFont			= mpEngine->LoadFont("res/HoboStd.otf", 46U);
	mpSpriteLogo	= mpEngine->CreateSprite("res/logo.png", 314.0f, 76.0f);
	mpSprBg			= mpEngine->CreateSprite("res/bg.jpg");

	int count = 0;

	for (auto map : mSettings.GetMaps()) {
		mItems[count] = { "Map " + to_string(count + 1), count };
		++count;
	}

	mSelected = 0;
	
	int i = 0;

	for (auto &item : mItems) {
		item.second.mPos = Vec2<>(512, i * 56 + 380);
		++i;
	}

	mStateChange = false;
	mReturnState = CState::Exit;
}

int CStateMapChooser::Run() {
	while (mpEngine->IsRunning() && !mStateChange) {
		if (mpEngine->KeyHit(Key_Escape)) {
			mpEngine->Stop();
			break;
		}

		if (mpEngine->KeyHit(Key_Down)) mSelected = (mSelected + 1) % mItems.size();
		if (mpEngine->KeyHit(Key_Up)) mSelected = (mSelected - 1) % mItems.size();

		if (mpEngine->KeyHit(Key_Return)) {
			mSettings.SetMap(mItems[mSelected].mTxt);
			mReturnState = CState::Pathfinder;
			mStateChange = true;
		}

		for (auto &item : mItems)
			mpFont->Draw(item.second.mTxt, item.second.mPos.x, item.second.mPos.y, (item.first == mSelected) ? kBlue : kBlack, kCentre, kVCentre);

		mpEngine->DrawScene();
	}

	mpEngine->RemoveFont(mpFont);
	mpEngine->RemoveSprite(mpSpriteLogo);
	mpEngine->RemoveSprite(mpSprBg);

	return mReturnState;
}