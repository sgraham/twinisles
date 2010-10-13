#include "lg/lg.h"
#include "render.h"

#include "mainpal_bin.h"
#include "fishing_left1_bin.h"
#include "fishing_left2_bin.h"
#include "fishing_right1_bin.h"
#include "fishing_right2_bin.h"
#include "fishing_down1_bin.h"
#include "fishing_down2_bin.h"
#include "fishing_up1_bin.h"
#include "fishing_up2_bin.h"
#include "cloud_rain_bin.h"
#include "cloud_storm_bin.h"
#include "cloud_tornado_bin.h"
#include "fish1_bin.h"
#include "fish2_bin.h"
#include "patrol_left1_bin.h"
#include "patrol_left2_bin.h"
#include "patrol_right1_bin.h"
#include "patrol_right2_bin.h"
#include "patrol_down1_bin.h"
#include "patrol_down2_bin.h"
#include "patrol_up1_bin.h"
#include "patrol_up2_bin.h"

using namespace luvafair;

Render::Render()
	: mFishingBoat(32, 32, mainpal_bin, 8)
	, mRainCloud(64, 64, mainpal_bin)
	, mStormCloud(64, 64, mainpal_bin)
	, mTornado(64, 64, mainpal_bin)
	, mFish(32, 32, mainpal_bin, 2)
	, mPirate(32, 32, mainpal_bin, 8)
{
	mFishingBoat.AddFrame(fishing_up1_bin);
	mFishingBoat.AddFrame(fishing_up2_bin);
	mFishingBoat.SetEndOfAnimation();
	mFishingBoat.AddFrame(fishing_down1_bin);
	mFishingBoat.AddFrame(fishing_down2_bin);
	mFishingBoat.SetEndOfAnimation();
	mFishingBoat.AddFrame(fishing_left1_bin);
	mFishingBoat.AddFrame(fishing_left2_bin);
	mFishingBoat.SetEndOfAnimation();
	mFishingBoat.AddFrame(fishing_right1_bin);
	mFishingBoat.AddFrame(fishing_right2_bin);
	mFishingBoat.SetEndOfAnimation();

	mRainCloud.AddFrame(cloud_rain_bin);
	mStormCloud.AddFrame(cloud_storm_bin);
	mTornado.AddFrame(cloud_tornado_bin);

	mFish.AddFrame(fish1_bin);
	mFish.AddFrame(fish2_bin);
	mFish.SetEndOfAnimation();

	mPirate.AddFrame(patrol_up1_bin);
	mPirate.AddFrame(patrol_up2_bin);
	mPirate.SetEndOfAnimation();
	mPirate.AddFrame(patrol_down1_bin);
	mPirate.AddFrame(patrol_down2_bin);
	mPirate.SetEndOfAnimation();
	mPirate.AddFrame(patrol_left1_bin);
	mPirate.AddFrame(patrol_left2_bin);
	mPirate.SetEndOfAnimation();
	mPirate.AddFrame(patrol_right1_bin);
	mPirate.AddFrame(patrol_right2_bin);
	mPirate.SetEndOfAnimation();
}

SpriteInstance* Render::CreateFishingBoat()
{
	return new SpriteInstance(&mFishingBoat);
}

SpriteInstance* Render::CreateRainCloud()
{
	return new SpriteInstance(&mRainCloud);
}

SpriteInstance* Render::CreateStormCloud()
{
	return new SpriteInstance(&mStormCloud);
}

SpriteInstance* Render::CreateTornado()
{
	return new SpriteInstance(&mTornado);
}

SpriteInstance* Render::CreateFish()
{
	return new SpriteInstance(&mFish);
}

SpriteInstance* Render::CreatePirate()
{
	return new SpriteInstance(&mPirate);
}


