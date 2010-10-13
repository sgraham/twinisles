#include "lg/lg.h"
#include "touch_smoother.h"
#include <cstring>

void TouchSmoother::Add(const touchPosition& tp)
{
	bool alreadyFull = false;
	if (mPositionCount == ARRAYSIZE(mPositions))
	{
		memmove(mPositions, mPositions + 1, (ARRAYSIZE(mPositions) - 1) * sizeof(V2));
		--mPositionCount;
		alreadyFull = true;
	}

	mPositions[mPositionCount++] = tp;
	if (!alreadyFull && mPositionCount == 2)
	{
		mPressed = true;
	}
	if (!alreadyFull && mPositionCount == ARRAYSIZE(mPositions))
	{
		mPressedSolid = true;
	}
}

void TouchSmoother::Update()
{
	scanKeys(); // this is a HW/arm7 thing

	if (mReleased)
	{
		mPositionCount = 0;
	}

	mPressed = false;
	mPressedSolid = false;
	mReleased = false;

	if (keysHeld() & KEY_TOUCH)
	{
		Add(touchReadXY());
/*
	consoleDemoInit();
	consoleClear();
	BG_PALETTE_SUB[0] = RGB15(0,0,0);
	BG_PALETTE_SUB[255] = RGB15(31,31,31);
	iprintf("\n");
	for (int i = 0; i < ARRAYSIZE(mPositions); ++i)
	{
		iprintf("%x %x\n", mPositions[i].x.v, mPositions[i].y.v);
	}
	*/
	}
	else if (keysUp() & KEY_TOUCH)
	{
		mReleased = true;
	}
}

bool TouchSmoother::Pressed() const
{
	return mPressed;
}

bool TouchSmoother::PressedSolid() const
{
	return mPressedSolid;
}

V2 TouchSmoother::GetPos() const
{
	assert(mPositionCount > 0);
	V2 avg;
	for (int i = 0; i < mPositionCount; ++i)
	{
		avg += mPositions[i];
	}
	avg /= mPositionCount;
	return avg;
}

bool TouchSmoother::Held() const
{
	return mPositionCount == ARRAYSIZE(mPositions);
}

bool TouchSmoother::Released() const
{
	return mReleased;
}

