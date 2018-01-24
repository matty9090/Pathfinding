#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Settings.hpp"
#include "State.hpp"

class CStateMapChooser : public CState {
	public:
		CStateMapChooser(tle::I3DEngine *engine, CSettings &settings);

		void Init();
		int Run();

	private:
		tle::IFont *mpFont;
		tle::ISprite *mpSprBg;
		tle::ISprite *mpSpriteLogo;

		struct MenuItem {
			std::string mTxt;
			int mMapID;
			Vec2<> mPos;
		};

		bool mStateChange;
		int mSelected, mReturnState;
		std::map<int, MenuItem> mItems;
};