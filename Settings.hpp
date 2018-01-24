#pragma once

#include <string>
#include <map>
#include <TL-Engine.h>

#include "JSON.hpp"
#include "Vec3.hpp"

/*
	Class to handle the settings file and store the settings data
*/

class CSettings {
	public:
		CSettings(std::string file);

		void SetMap(std::string map_id) { mCurrentMap = map_id; }
		size_t GetScreenW() { return mWindowW; }
		size_t GetScreenH() { return mWindowH; }
		float GetMapScale() { return mMapScale; }
		std::string GetMapsFolder() { return mMapsFolder; }

		// Encapsulate a model
		struct SModel {
			std::string mFile, mTex;
			Vec3<> mPos;
			bool mInst;
			float mScale;
		};

		// Encapsulate a map description
		struct SMap {
			std::string mMapFile, mCoordsFile;
			Vec2<size_t> mDims;
		};

		// Encapsulate a particular key
		struct SKeyDesc {
			std::string mDesc;
			tle::EKeyCode mCode;
		};

		std::map<std::string, SModel> &GetModels() { return mModels; }
		std::map<std::string, SMap> &GetMaps() { return mMaps; }
		std::map<std::string, SKeyDesc> &GetKeys() { return mKeys; }
		tle::EKeyCode GetKeyCode(std::string txt) { return mKeys[txt].mCode; }

		SMap &GetCurrentMap() { return mMaps[mCurrentMap]; }
		Vec2<size_t> GetMapSize() { return Vec2<size_t>(10, 10); }

		int getMenuSpacing() { return mMenuSpacing; }
		Vec2<float> getLogoPos() { return mLogoPos; }
		Vec2<float> getMenuPos() { return mMenuPos; }
		size_t getMenuFontSize() { return mMenuFontSize; }

	private:
		picojson::value mJson;

		float mMapScale;
		size_t mWindowW, mWindowH, mMenuFontSize;
		std::string mMapsFolder, mCurrentMap;

		int mMenuSpacing;
		Vec2<float> mLogoPos, mMenuPos;
		
		std::map<std::string, SModel> mModels;
		std::map<std::string, SMap> mMaps;
		std::map<std::string, SKeyDesc> mKeys;

		void LoadProperties();
		void LoadModels();
		void LoadMaps();
		void LoadKeys();
};