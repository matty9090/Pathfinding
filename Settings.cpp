/// Author: Matthew Lowe
/// Copyright (c) 2018

#include "Settings.hpp"

#include <fstream>

using namespace std;

CSettings::CSettings(string file)
{
	// Parse the json file
	ifstream f(file);
	picojson::parse(mJson, f);

	// Load and process all aspects of the settings
	LoadProperties();
	LoadModels();
	LoadMaps();
	LoadKeys();

	f.close();
}

// Load the general constants
void CSettings::LoadProperties()
{
	mWindowW = (size_t)mJson.get("window_w").get<double>();
	mWindowH = (size_t)mJson.get("window_h").get<double>();

	mMapScale	= (float)mJson.get("map_scale").get<double>();
	mMapsFolder	= mJson.get("maps_folder").get<string>();

	mMenuPos		= Vec2<float>((float)mJson.get("menu_items_x").get<double>(), (float)mJson.get("menu_items_y").get<double>());
	mLogoPos		= Vec2<float>((float)mJson.get("logo_x").get<double>(), (float)mJson.get("logo_y").get<double>());
	mMenuSpacing	= (int)mJson.get("menu_items_spacing").get<double>();
	mMenuFontSize	= (size_t)mJson.get("menu_font_size").get<double>();
	mGameFontSize   = (size_t)mJson.get("game_font_size").get<double>();
	mHoverHeight	= (float)mJson.get("node_hover_height").get<double>();
	mStepSpeed		= (float)mJson.get("step_speed").get<double>();
	mBezierSteps	= (int)mJson.get("bezier_steps").get<double>();
	mCatmullSteps	= (int)mJson.get("catmull_steps").get<double>();

	mOrigin.x = (float)mJson.get("origin_x").get<double>();
	mOrigin.y = (float)mJson.get("origin_y").get<double>();
	mOrigin.z = (float)mJson.get("origin_z").get<double>();

	mCameraPos.x = (float)mJson.get("cam_x").get<double>();
	mCameraPos.y = (float)mJson.get("cam_y").get<double>();
	mCameraPos.z = (float)mJson.get("cam_z").get<double>();
	mCameraRot   = (float)mJson.get("cam_rot").get<double>();

	mKeysPos.x = (float)mJson.get("ui_keys_x").get<double>();
	mKeysPos.y = (float)mJson.get("ui_keys_y").get<double>();
	mInfoPos.x = (float)mJson.get("ui_info_x").get<double>();
	mInfoPos.y = (float)mJson.get("ui_info_y").get<double>();

	mNodeCosts["Wall"]  = (int)mJson.get("id_wall").get<double>();
	mNodeCosts["Clear"] = (int)mJson.get("id_wall").get<double>();
	mNodeCosts["Wood"]  = (int)mJson.get("id_wall").get<double>();
	mNodeCosts["Water"] = (int)mJson.get("id_wall").get<double>();
}

// Load the list of models needed
void CSettings::LoadModels()
{
	picojson::array modelList = mJson.get("models").get<picojson::array>();

	cout << "Models:\n\n";

	for (auto model : modelList)
	{
		// Read values from JSON file
		string name = model.get("name").get<string>();
		string file = model.get("file").get<string>();
		string tex  = "";
		float scale = 1.0f;

		cout << "\t" << name << " (" << file << ")";
		
		// Output debug info
		if (!model.get("tex").is<picojson::null>())
		{
			tex = model.get("tex").get<string>();
			cout << " using texture " << tex;
		}

		// Output debug info
		if (!model.get("scale").is<picojson::null>())
		{
			scale = (float)model.get("scale").get<double>();
			cout << " scaled to " << scale;
		}

		// If coordinates exist this the model only needed 1 instance
		if (!model.get("x").is<picojson::null>())
		{
			float x = (float)model.get("x").get<double>();
			float y = (float)model.get("y").get<double>();
			float z = (float)model.get("z").get<double>();

			mModels[name].mInst = true;
			mModels[name].mPos  = Vec3<>(x, y, z);
		}
		
		// Save the data
		mModels[name].mTex = tex;
		mModels[name].mFile = file;
		mModels[name].mScale = scale;

		cout << "\n";
	}

	cout << "\n";
}

// Load the map descriptions
void CSettings::LoadMaps() {
	picojson::array mapList = mJson.get("maps").get<picojson::array>();

	cout << "Maps:\n\n";

	for (auto m : mapList)
	{
		string name  = m.get("name").get<string>();
		string mfile = m.get("map_file").get<string>();
		string cfile = m.get("coords_file").get<string>();
		size_t mapW = (size_t)m.get("map_width").get<double>();
		size_t mapH = (size_t)m.get("map_height").get<double>();

		Vec2<size_t> dims(mapW, mapH);

		// Save the descriptions
		mMaps[name].mMapFile = mfile;
		mMaps[name].mCoordsFile = cfile;
		mMaps[name].mDims = dims;

		// Debug info
		cout << "\t" << name << " " << dims.toString();
		cout << " - " << mfile << ", " << cfile << "\n";
	}

	cout << "\n";
}

// Load the keys
void CSettings::LoadKeys()
{
	picojson::array keysList = mJson.get("keys").get<picojson::array>();
	
	for (auto key : keysList)
	{
		string use = key.get("use").get<string>();
		string txt = key.get("txt").get<string>();

		// Turn into TL-Engine EKeyCode from integer value
		tle::EKeyCode code = (tle::EKeyCode)((int)key.get("code").get<double>());

		string desc = use + ": " + txt;

		mKeys[use] = { desc, code };
	}
}