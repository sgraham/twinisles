#ifndef INCLUDED_render_H
#define INCLUDED_render_H

#include "lg/sprite.h"

class Render
{
	luvafair::SpriteData mFishingBoat;
	luvafair::SpriteData mRainCloud;
	luvafair::SpriteData mStormCloud;
	luvafair::SpriteData mTornado;
	luvafair::SpriteData mFish;
	luvafair::SpriteData mPirate;

	public:
		Render();
		luvafair::SpriteInstance* CreateFishingBoat();
		luvafair::SpriteInstance* CreateRainCloud();
		luvafair::SpriteInstance* CreateStormCloud();
		luvafair::SpriteInstance* CreateTornado();
		luvafair::SpriteInstance* CreateFish();
		luvafair::SpriteInstance* CreatePirate();
};

#endif
