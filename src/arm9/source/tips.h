#ifndef LG_INCLUDED_tips_H
#define LG_INCLUDED_tips_H

namespace luvafair { class Font; }

enum TipType
{
	Tip_Invalid = -1,

	Tip_First, 

	// note: continuations of multi-screens need to go at the top so they're not interrupted
	// by new tips getting added.
	Tip_DoResearch2 = Tip_First,
	Tip_DoResearch3,
	Tip_CompletedFirstResearch2,

	Tip_Welcome1,
	Tip_Welcome2,
	Tip_Welcome3,

	Tip_YourIsland,
	Tip_FirstBuildings,
	Tip_InterfaceMove,
	Tip_CropsNearWater,

	Tip_DoResearch,

	Tip_CompletedFirstResearch,

	Tip_AboutTornados,

	Tip_TechUp,

	Tip_OnlyOneBuilding,

	Tip_School,

	Tip_BulldozerBeforeBuilding,

	Tip_Count
};

class Tips
{
	luvafair::Font& mFont;
	bool mGloballyEnabled;
	bool mHaveDisplayed[Tip_Count];
	bool mPending[Tip_Count];
	TipType mDisplaying;

	void Draw(u16* bg);

public:
	Tips(luvafair::Font& font, bool disable = false);
	void Add(TipType tip);
	bool Update(u16* bg, u16 down);
	void Dismiss(u16* bg);
	bool IsDisplaying() const { return mDisplaying != Tip_Invalid; }
	bool HaveDisplayed(TipType tip) { return !mGloballyEnabled || mHaveDisplayed[tip]; }
	void ClearAllPending();
};

#endif
