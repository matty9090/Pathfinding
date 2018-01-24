#pragma once

#include <string>
#include <map>
#include <TL-Engine.h>

#include "JSON.hpp"
#include "Vec3.hpp"

class CSettings {
	public:
		CSettings(std::string file);

		void SetMap(std::string map_id) { mCurrentMap = map_id; }
		size_t GetScreenW() { return mWindowW; }
		size_t GetScreenH() { return mWindowH; }
		float GetMapScale() { return mMapScale; }
		std::string GetMapsFolder() { return mMapsFolder; }

		struct SModel {
			std::string mFile, mTex;
			Vec3<> mPos;
			bool mInst;
			float mScale;
		};

		struct SMap {
			std::string mMapFile, mCoordsFile;
			Vec2<size_t> mDims;
		};

		struct SKeyDesc {
			std::string mDesc;
			tle::EKeyCode mCode;
		};

		std::map<std::string, SModel> &GetModels() { return mModels; }
		std::map<std::string, SMap> &GetMaps() { return mMaps; }
		std::map<std::string, SKeyDesc> &GetKeys() { return mKeys; }
		tle::EKeyCode GetKeyCode(std::string txt) { return mKeys[txt].mCode; }

		SMap &getCurrentMap() { return mMaps[mCurrentMap]; }

	private:
		picojson::value mJson;

		float mMapScale;
		size_t mWindowW, mWindowH;
		std::string mMapsFolder, mCurrentMap;
		
		std::map<std::string, SModel> mModels;
		std::map<std::string, SMap> mMaps;
		std::map<std::string, SKeyDesc> mKeys;

		void LoadProperties();
		void LoadModels();
		void LoadMaps();
		void LoadKeys();
};