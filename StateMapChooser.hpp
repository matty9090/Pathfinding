#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Settings.hpp"
#include "State.hpp"

class StateMapChooser : public State {
	public:
		StateMapChooser(tle::I3DEngine *engine, Settings &settings);

		void init();
		int run();

	private:
		tle::IFont *font;
		tle::ISprite *spr_bg;
		tle::ISprite *spr_logo;

		struct MenuItem {
			std::string txt;
			int map_id;
			Vec2<> pos;
		};

		bool state_change;
		int selected, return_state;
		std::map<int, MenuItem> items;
};