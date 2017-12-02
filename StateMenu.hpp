#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Settings.hpp"
#include "State.hpp"

class StateMenu : public State {
	public:
		StateMenu(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

	private:
		typedef void(StateMenu::*MenuAction)();

		tle::IFont *font;
		tle::ISprite *spr_bg;
		tle::ISprite *spr_logo;

		struct MenuItem {
			std::string txt;
			MenuAction action;
			Vec2<> pos;
		};

		bool state_change;
		int selected, return_state;
		std::map<int, MenuItem> items;

		void action_select();
		void action_exit();
};