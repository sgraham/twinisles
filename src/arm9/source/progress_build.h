#ifndef LG_INCLUDED_progress_build_H
#define LG_INCLUDED_progress_build_H

#include "memory_config.h"
class V2;
#include "lg/sprite.h"
#include "buildings.h"
class WorldObject;
class ObjectMgr;

class ProgressBuild
{
	u8 mRawDataBuilding[LG_ART_SQUARE_SIZE * LG_ART_SQUARE_SIZE];
	u8 mRawDataMeter[LG_ART_SQUARE_SIZE * LG_ART_SQUARE_SIZE];
	luvafair::SpriteData mSpriteDataBuilding;
	luvafair::SpriteData mSpriteDataMeter;
	luvafair::SpriteInstance mSpriteInstanceBuilding;
	luvafair::SpriteInstance mSpriteInstanceMeter;
	WorldObject* mWorldObjectBuilding;
	WorldObject* mWorldObjectMeter;
	const u8* mData;

	u8 mMeterBackground;
	u8 mMeterForeground;

	u8 FindClosestColourTo(int r, int g, int b);
	void FindMeterColours();

	public:
		ProgressBuild(ObjectMgr& objectMgr);

		void StartBuilding(const u8* data, const V2& mapLocation);
		void SetPercentage(luvafair::F32 percent);

		void Show();
		void Hide();
};

#endif
