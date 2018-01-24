#include "Settings.hpp"

#include <fstream>

using namespace std;

CSettings::CSettings(string file) {
	ifstream f(file);
	picojson::parse(mJson, f);

	LoadProperties();
	LoadModels();
	LoadMaps();
	LoadKeys();

	f.close();

	mCurrentMap = "Map 2";
}

void CSettings::LoadProperties() {
	mWindowW = (size_t)mJson.get("window_w").get<double>();
	mWindowH = (size_t)mJson.get("window_h").get<double>();

	mMapScale	= (float)mJson.get("map_scale").get<double>();
	mMapsFolder	= mJson.get("maps_folder").get<string>();
}

void CSettings::LoadModels() {
	picojson::array modelList = mJson.get("models").get<picojson::array>();

	cout << "Models:\n\n";

	for (auto model : modelList) {
		string name = model.get("name").get<string>();
		string file = model.get("file").get<string>();
		string tex  = "";
		float scale = 1.0f;

		cout << "\t" << name << " (" << file << ")";
		
		if (!model.get("tex").is<picojson::null>()) {
			tex = model.get("tex").get<string>();
			cout << " using texture " << tex;
		}

		if (!model.get("scale").is<picojson::null>()) {
			scale = (float)model.get("scale").get<double>();
			cout << " scaled to " << scale;
		}

		if (!model.get("x").is<picojson::null>()) {
			float x = (float)model.get("x").get<double>();
			float y = (float)model.get("y").get<double>();
			float z = (float)model.get("z").get<double>();

			mModels[name].mInst = true;
			mModels[name].mPos  = Vec3<>(x, y, z);
		}
		
		mModels[name].mTex = tex;
		mModels[name].mFile = file;
		mModels[name].mScale = scale;

		cout << "\n";
	}

	cout << "\n";
}

void CSettings::LoadMaps() {
	picojson::array mapList = mJson.get("maps").get<picojson::array>();

	cout << "Maps:\n\n";

	for (auto model : mapList) {
		string name  = model.get("name").get<string>();
		string mfile = model.get("map_file").get<string>();
		string cfile = model.get("coords_file").get<string>();
		size_t mapW = (size_t)model.get("map_width").get<double>();
		size_t mapH = (size_t)model.get("map_height").get<double>();

		Vec2<size_t> dims(mapW, mapH);

		mMaps[name].mMapFile = mfile;
		mMaps[name].mCoordsFile = cfile;
		mMaps[name].mDims = dims;

		cout << "\t" << name << " " << dims.toString();
		cout << " - " << mfile << ", " << cfile << "\n";
	}

	cout << "\n";
}

void CSettings::LoadKeys() {
	picojson::array keysList = mJson.get("keys").get<picojson::array>();
	
	for (auto key : keysList) {
		string use = key.get("use").get<string>();
		string txt = key.get("txt").get<string>();
		tle::EKeyCode code = (tle::EKeyCode)((int)key.get("code").get<double>());

		string desc = use + ": " + txt;

		mKeys[use] = { desc, code };
	}
}