#include "StateMenu.hpp"
#include "MapLoader.hpp"

#include <Windows.h>
#include <Commdlg.h>

using namespace tle;

StateMenu::StateMenu(tle::I3DEngine *engine, Settings &settings) : State(engine, settings) {
	
}

void StateMenu::init() {
	font		= engine->LoadFont("res/HoboStd.otf", 46U);
	spr_logo	= engine->CreateSprite("res/logo.png", 314.0f, 76.0f);
	spr_bg		= engine->CreateSprite("res/bg.jpg");

	items[0] = { "Choose map", &StateMenu::action_select };
	items[1] = { "Exit", &StateMenu::action_exit };

	selected = 0;
	
	int i = 0;

	for (auto &item : items) {
		item.second.pos = Vec2<>(512, i * 56 + 380);
		++i;
	}

	state_change = false;
	return_state = State::Exit;
}

int StateMenu::run() {
	while (engine->IsRunning() && !state_change) {
		if (engine->KeyHit(Key_Escape)) {
			engine->Stop();
			break;
		}

		if (engine->KeyHit(Key_Down)) selected = (selected + 1) % items.size();
		if (engine->KeyHit(Key_Up)) selected = (selected - 1) % items.size();

		if (engine->KeyHit(Key_Return)) ((*this).*items[selected].action)();

		for (auto &item : items)
			font->Draw(item.second.txt, item.second.pos.x, item.second.pos.y, (item.first == selected) ? kBlue : kBlack, kCentre, kVCentre);

		engine->DrawScene();
	}

	return return_state;
}

void StateMenu::action_select() {
	OPENFILENAMEA inf;
	ZeroMemory(&inf, sizeof(inf));
	char file[100];
	
	inf.lStructSize = sizeof(inf);
	inf.hwndOwner = NULL;
	inf.lpstrFile = file;
	inf.lpstrFile[0] = '\0';
	inf.nMaxFile = sizeof(file);
	inf.lpstrFilter = "All\0";
	inf.nFilterIndex = 1;
	inf.lpstrFileTitle = NULL;
	inf.nMaxFileTitle = 0;
	inf.lpstrInitialDir = NULL;
	inf.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	GetOpenFileNameA(&inf);
}

void StateMenu::action_exit() {
	state_change = true;
	return_state = State::Exit;
}