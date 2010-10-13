#ifndef LG_INCLUDED_touch_smoother_H
#define LG_INCLUDED_touch_smoother_H

#include "lg/v2.h"

class TouchSmoother
{
	V2 mPositions[4];
	int mPositionCount;
	bool mPressed;
	bool mPressedSolid;
	bool mReleased;

	void Add(const touchPosition& tp);

public:
	TouchSmoother() : mPositionCount(0), mPressed(false), mPressedSolid(false), mReleased(false) {}

	void Update();

	bool Pressed() const;
	bool PressedSolid() const;
	V2 GetPos() const;
	bool Held() const;
	bool Released() const;
};

#endif
