#pragma once

#include <TL-Engine.h>
#include <map>
#include <memory>
#include <vector>

#include "State.hpp"
#include "Settings.hpp"

class CApp {
	public:
		CApp(CSettings &settings);
		~CApp();

		void run();

	private:
		int mState;
		std::vector<std::unique_ptr<CState>> mStates;

		CSettings &mSettings;
		tle::I3DEngine *mEng;

		std::map<string, tle::IModel*> mModels;
};