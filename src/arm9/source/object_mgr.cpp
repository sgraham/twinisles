#include "lg/lg.h"
#include "object_mgr.h"

using namespace luvafair;

WorldObject::~WorldObject()
{
	if (mOwnSI) delete mSI;
}

ObjectMgr::ObjectMgr() : mNumObjects(0)
{
}

WorldObject* ObjectMgr::Add(const V2& pos, SpriteInstance* si, bool ownSI)
{
	WorldObject* wo = &mObjects[mNumObjects++];
	wo->pos = pos;
	wo->SetSpriteInstance(si);
	wo->mOwnSI = ownSI;
	return wo;
}

void ObjectMgr::Update(const V2& scrollLoc)
{
	for (int i = 0; i < mNumObjects; ++i)
	{
		if (mObjects[i].IsHidden()) continue;

		V2 pos = mObjects[i].pos - scrollLoc;
		int w = mObjects[i].GetSpriteInstance()->GetData()->GetWidth();
		int h = mObjects[i].GetSpriteInstance()->GetData()->GetHeight();
		if (pos.x.Int() + w/2 <= 0 || pos.x.Int() - w/2 >= SCREEN_WIDTH
			|| pos.y.Int() + h/2 <= 0 || pos.y.Int() - h/2 >= SCREEN_HEIGHT)
		{
			mObjects[i].GetSpriteInstance()->SetPosition(SCREEN_WIDTH, SCREEN_HEIGHT);
		}
		else
		{
			mObjects[i].GetSpriteInstance()->SetPosition(V2(pos.x.Int() - w/2, pos.y.Int() - h/2));
		}
	}
}
