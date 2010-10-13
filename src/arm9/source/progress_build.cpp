#include "lg/lg.h"
#include "progress_build.h"
#include "mainpal_bin.h"
#include "object_mgr.h"
#include <cstring>

using namespace luvafair;

u8 ProgressBuild::FindClosestColourTo(int r, int g, int b)
{
	u16* pal = (u16*)mainpal_bin;
	u8 closest = 0;
	int closestDistSq = 99999;
	for (int i = 0; i < 256; ++i)
	{
		int pr = pal[i] & 0x1f;
		int pg = (pal[i] & (0x1f << 5)) >> 5;
		int pb = (pal[i] & (0x1f << 10)) >> 10;

		// this is wrong-ish; should be in hsv, but good enough for our purposes
		int distsq = (pr - r) * (pr - r)
			+ (pg - g) * (pg - g)
			+ (pb - b) * (pb - b);

		if (distsq < closestDistSq)
		{
			closest = (u8)i;
			closestDistSq = distsq;
		}
	}
	return closest;
}

void ProgressBuild::FindMeterColours()
{
	mMeterBackground = FindClosestColourTo(10, 10, 10);
	mMeterForeground = FindClosestColourTo(31, 0, 0);
}

ProgressBuild::ProgressBuild(ObjectMgr& objectMgr)
	: mSpriteDataBuilding(LG_ART_SQUARE_SIZE, LG_ART_SQUARE_SIZE, mainpal_bin)
	, mSpriteDataMeter(LG_ART_SQUARE_SIZE, LG_ART_SQUARE_SIZE, mainpal_bin)
	, mSpriteInstanceBuilding(&mSpriteDataBuilding)
	, mSpriteInstanceMeter(&mSpriteDataMeter)
	, mWorldObjectBuilding(objectMgr.Add(V2(0,0), &mSpriteInstanceBuilding))
	, mWorldObjectMeter(objectMgr.Add(V2(0,0), &mSpriteInstanceMeter))
{
	mWorldObjectBuilding->Hide();
	mWorldObjectMeter->Hide();

	FindMeterColours();
}

void ProgressBuild::StartBuilding(const u8* data, const V2& mapLocation)
{
	mData = data;
	mWorldObjectBuilding->pos = mapLocation * LG_ART_SQUARE_SIZE + (LG_ART_SQUARE_SIZE / 2);
	mWorldObjectMeter->pos = mapLocation * LG_ART_SQUARE_SIZE + (LG_ART_SQUARE_SIZE / 2) + V2(0, 4);
	memset(mRawDataBuilding, 0, sizeof(mRawDataBuilding));
	memset(mRawDataMeter, 0, sizeof(mRawDataMeter));
	DC_FlushRange(mRawDataBuilding, sizeof(mRawDataBuilding));
	DC_FlushRange(mRawDataMeter, sizeof(mRawDataMeter));
}

static int LinearToTiled(int x, int y, int stride)
{
	int tx = x / 8;
	int ty = y / 8;
	return (ty * (stride / 8) + tx) * (8*8) + (y%8) * 8 + (x%8);
}

void ProgressBuild::SetPercentage(F32 percent)
{
	memset(mRawDataBuilding, 0, sizeof(mRawDataBuilding));
	memset(mRawDataMeter, 0, sizeof(mRawDataMeter));
	int count = (F32(ARRAYSIZE(mRawDataBuilding)) * percent / F32(100)).Int();
	assert(mData != 0);
	assert(count < ARRAYSIZE(mRawDataBuilding));
	for (int i = 0; i < count; ++i)
	{
		mRawDataBuilding[i] = mData[i];
	}
	for (int x = 0; x < LG_ART_SQUARE_SIZE; ++x)
	{
		u8 clr = F32(x) < (percent * LG_ART_SQUARE_SIZE / 100) ? mMeterForeground : mMeterBackground;
		for (int y = LG_ART_SQUARE_SIZE - 4; y < LG_ART_SQUARE_SIZE; ++y)
		{
			mRawDataMeter[LinearToTiled(x, y, LG_ART_SQUARE_SIZE)] = clr;
		}
	}
	DC_FlushRange(mRawDataBuilding, sizeof(mRawDataBuilding));
	mSpriteDataBuilding.UpdateFrame(0, mRawDataBuilding);
	mSpriteDataMeter.UpdateFrame(0, mRawDataMeter);
}

void ProgressBuild::Show()
{
	mWorldObjectBuilding->Show();
	mWorldObjectMeter->Show();
}

void ProgressBuild::Hide()
{
	mWorldObjectBuilding->Hide();
	mWorldObjectMeter->Hide();
}
