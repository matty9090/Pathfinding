#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Settings.hpp"
#include "State.hpp"

/*
	Menu state
	Draws menu items
*/

class CStateMenu : public CState {
	public:
		CStateMenu(tle::I3DEngine *engine, CSettings &settings);

		void Init();
		int Run();

	private:
		typedef void(CStateMenu::*MenuAction)();

		tle::IFont *mpFont;
		tle::ISprite *mpSprBg;
		tle::ISprite *mpSprLogo;

		// Each menu item has various info needed to be stored
		struct MenuItem {
			std::string mTxt;
			MenuAction mAction;
			Vec2<> mPos;
		};

		bool mStateChange;
		int mSelected, mReturnState;
		std::map<int, MenuItem> mItems;

		void ActionSelect();
		void ActionExit();
};