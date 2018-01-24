#pragma once

#include <string>
#include <map>
#include <TL-Engine.h>

#include "JSON.hpp"
#include "Vec3.hpp"

/*
	Class to handle the settings file and store the settings data
*/

class CSettings
{
	public:
		CSettings(std::string file);

		void SetMap(std::string map_id) { mCurrentMap = map_id; }
		size_t GetScreenW() { return mWindowW; }
		size_t GetScreenH() { return mWindowH; }
		float GetMapScale() { return mMapScale; }
		std::string GetMapsFolder() { return mMapsFolder; }

		// Encapsulate a model
		struct SModel
		{
			std::string mFile, mTex;
			Vec3<> mPos;
			bool mInst;
			float mScale;
		};

		// Encapsulate a map description
		struct SMap
		{
			std::string mMapFile, mCoordsFile;
			Vec2<size_t> mDims;
		};

		// Encapsulate a particular key
		struct SKeyDesc
		{
			std::string mDesc;
			tle::EKeyCode mCode;
		};

		std::map<std::string, SModel> &GetModels() { return mModels; }
		std::map<std::string, SMap> &GetMaps() { return mMaps; }
		std::map<std::string, SKeyDesc> &GetKeys() { return mKeys; }
		std::map<string, int> GetNodeCosts() { return mNodeCosts; }
		tle::EKeyCode GetKeyCode(std::string txt) { return mKeys[txt].mCode; }

		SMap &GetCurrentMap() { return mMaps[mCurrentMap]; }
		Vec2<size_t> GetMapSize() { return Vec2<size_t>(10, 10); }

		int GetMenuSpacing() { return mMenuSpacing; }
		int GetBezierSteps() { return mBezierSteps; }
		int GetCatmullSteps() { return mCatmullSteps; }
		float GetStepSpeed() { return mStepSpeed; }
		float GetCamRot() { return mCameraRot; }
		float GetHoverHeight() { return mHoverHeight; }
		size_t GetMenuFontSize() { return mMenuFontSize; }
		size_t GetGameFontSize() { return mGameFontSize; }
		Vec2<float> GetLogoPos() { return mLogoPos; }
		Vec2<float> GetMenuPos() { return mMenuPos; }
		Vec2<float> GetKeysPos() { return mKeysPos; }
		Vec2<float> GetInfoPos() { return mInfoPos; }
		Vec3<> GetOrigin() { return mOrigin; }
		Vec3<> GetCamPos() { return mCameraPos; }

	private:
		picojson::value mJson;

		float mMapScale, mStepSpeed, mCameraRot, mHoverHeight;
		size_t mWindowW, mWindowH;
		size_t mMenuFontSize, mGameFontSize;
		std::string mMapsFolder, mCurrentMap;
		std::map<string, int> mNodeCosts;

		int mMenuSpacing;
		int mBezierSteps, mCatmullSteps;
		Vec2<float> mLogoPos, mMenuPos, mKeysPos, mInfoPos;
		Vec3<> mOrigin, mCameraPos;
		
		std::map<std::string, SModel> mModels;
		std::map<std::string, SMap> mMaps;
		std::map<std::string, SKeyDesc> mKeys;

		void LoadProperties();
		void LoadModels();
		void LoadMaps();
		void LoadKeys();
};