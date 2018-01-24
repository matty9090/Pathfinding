#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Settings.hpp"
#include "State.hpp"

/*
	State to list maps so the user can select them
*/

class CStateMapChooser : public CState {
	public:
		CStateMapChooser(tle::I3DEngine *engine, CSettings &settings);

		void Init();
		int Run();

	private:
		tle::IFont *mpFont;
		tle::ISprite *mpSprBg;
		tle::ISprite *mpSpriteLogo;

		// Each menu item has various info needed to be stored
		struct MenuItem {
			std::string mTxt;
			int mMapID;
			Vec2<> mPos;
		};

		bool mStateChange;
		int mSelected, mReturnState;
		std::map<int, MenuItem> mItems;
};