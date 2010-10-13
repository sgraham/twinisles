#ifndef INCLUDED_object_mgr_H
#define INCLUDED_object_mgr_H

#include "lg/sprite.h"
#include "lg/v2.h"

class WorldObject
{

	public:
		V2 pos;
	private:
		luvafair::SpriteInstance* mSI;
		bool mIsHidden;
		bool mOwnSI;
	public:
		WorldObject() : pos(0, 0), mSI(0), mIsHidden(false), mOwnSI(false) {}
		~WorldObject();

		luvafair::SpriteInstance* GetSpriteInstance() const { assert(mSI); return mSI; }
		void SetSpriteInstance(luvafair::SpriteInstance* si) { assert(si); mSI = si; }
		void Hide() { mSI->SetPosition(V2(SCREEN_WIDTH, SCREEN_HEIGHT)); mIsHidden = true; }
		void Show() { mSI->SetPosition(pos); mIsHidden = false; }

		bool IsHidden() const { return mIsHidden; }

		friend class ObjectMgr;
};

// keeps sprites at correct location when screen is scrolled
class ObjectMgr
{
	WorldObject mObjects[128];
	int mNumObjects;

	public:
		ObjectMgr();
		WorldObject* Add(const V2& loc, luvafair::SpriteInstance* spr, bool ownSI = false);
		void Update(const V2& scrollLoc);
};

#endif
