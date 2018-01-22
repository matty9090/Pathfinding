#include "StateMapChooser.hpp"
#include "MapLoader.hpp"

using namespace tle;

StateMapChooser::StateMapChooser(tle::I3DEngine *engine, Settings &settings) : State(engine, settings) {
	
}

void StateMapChooser::init() {
	font		= engine->LoadFont("res/HoboStd.otf", 46U);
	spr_logo	= engine->CreateSprite("res/logo.png", 314.0f, 76.0f);
	spr_bg		= engine->CreateSprite("res/bg.jpg");

	int count = 0;

	for (auto map : settings.getMaps()) {
		items[count] = { "Map " + to_string(count + 1), count };
		++count;
	}

	selected = 0;
	
	int i = 0;

	for (auto &item : items) {
		item.second.pos = Vec2<>(512, i * 56 + 380);
		++i;
	}

	state_change = false;
	return_state = State::Exit;
}

int StateMapChooser::run() {
	while (engine->IsRunning() && !state_change) {
		if (engine->KeyHit(Key_Escape)) {
			engine->Stop();
			break;
		}

		if (engine->KeyHit(Key_Down)) selected = (selected + 1) % items.size();
		if (engine->KeyHit(Key_Up)) selected = (selected - 1) % items.size();

		if (engine->KeyHit(Key_Return)) {
			settings.setMap(items[selected].txt);
			return_state = State::Pathfinder;
			state_change = true;
		}

		for (auto &item : items)
			font->Draw(item.second.txt, item.second.pos.x, item.second.pos.y, (item.first == selected) ? kBlue : kBlack, kCentre, kVCentre);

		engine->DrawScene();
	}

	engine->RemoveFont(font);
	engine->RemoveSprite(spr_logo);
	engine->RemoveSprite(spr_bg);

	return return_state;
}