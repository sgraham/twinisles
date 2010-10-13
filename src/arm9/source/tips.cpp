#include "lg/lg.h"
#include "tips.h"
#include "lg/draw.h"
#include "lg/font.h"
#include "sound_effects.h"
#include <cstring>

using namespace luvafair;

Tips::Tips(Font& font, bool disable)
	: mFont(font)
	, mGloballyEnabled(!disable)
	, mDisplaying(Tip_Invalid)
{
	memset(mHaveDisplayed, 0, sizeof(mHaveDisplayed));
	memset(mPending, 0, sizeof(mPending));

}

void Tips::Add(TipType tip)
{
	if (mHaveDisplayed[tip]) return;

	mPending[tip] = true;
}

void Tips::Draw(u16* bg)
{
	DrawFilledRect(bg, 10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, 0x8000 | RGB15(10, 10, 10));

	const char* str = "";
	bool multi = false;
	switch (mDisplaying)
	{
		case Tip_Welcome1:
			str = "Welcome to Twin Isles!\n"
				  "\n"
				  "Twin Isles is a simulation of an island\n"
				  "where you control the economics,\n"
				  "research, agriculture, and\n"
				  "infrastructure. Your goal is to grow\n"
				  "a happy population while overcoming\n"
				  "adversities along the way.";
			break;

		case Tip_Welcome2:
			str = "When your population exceeds your\n"
				  "'Food support' (top screen) your\n"
				  "people will be unhappy. You can build\n"
				  "Crops to provide food.\n"
				  "\n"
				  "You must also provide housing\n"
				  "for your people. Housing is provided\n"
				  "by building Huts.\n";
			break;

		case Tip_Welcome3:
			str = "You should now build a Hut and a field\n"
				  "of Crops.\n"
				  "\n"
				  "To bring up the build menu, dismiss\n"
				  "this window by tapping, then bring\n"
				  "up the Build menu by pressing \x87.\n"
				  "Finally, choose what to build and then\n"
				  "tap on the map to start building.";
			break;

		case Tip_YourIsland:
			str = "When you tap to place a building\n"
				  "you may only build on your island's\n"
				  "land.\n"
				  "\n"
				  "In this case, since you are Player 1\n"
				  "your island is the island on the left.";
			break;

		case Tip_FirstBuildings:
			str = "Good! You now have your first food\n"
				  "and housing. Your people will start\n"
				  "to get happier now and slowly your\n"
				  "population will grow.\n"
				  "\n"
				  "You can keep an eye on how happy\n"
				  "your people are by watching the mood\n"
				  "of the face on the right of the top\n"
				  "screen.";
			break;

		case Tip_InterfaceMove:
			str = "You should now continue adding food\n"
				  "and housing, to help your population\n"
				  "grow larger.\n"
				  "\n"
				  "In case you hadn't noticed yet, you\n"
				  "can move around the world by\n"
				  "dragging with the stylus, or by\n"
				  "using \x89.";
			break;

		case Tip_CropsNearWater:
			str = "One of your Crops just died, and so\n"
				  "your food support has decreased.\n"
				  "\n"
				  "Crops are more hardy when they're\n"
				  "placed adjacent to water so it's a\n"
				  "good strategy to build them on the\n"
				  "shores of your island.";
			break;

		case Tip_DoResearch:
			str = "An important part of strategy in\n"
				  "Twin Isles is research. Spending your\n"
				  "production on research is an\n"
				  "investment, which reaps rewards\n"
				  "such as new buildings, higher\n"
				  "production, and other bonuses.\n"
				  "\n"
				  "How fast you can construct new\n"
				  "buildings and conduct research\n";
			multi = true;
			Add(Tip_DoResearch2);
			break;

		case Tip_DoResearch2:
			str = "is controlled by your island's\n"
				  "production. Production is a measure\n"
				  "of your population's output.\n"
				  "\n"
				  "You can only build one building and\n"
				  "research one technology at a time.\n"
				  "If you're building and researching at\n"
				  "the same time the speed of both will\n"
				  "be half as your production is spread.\n";
			multi = true;
			Add(Tip_DoResearch3);
			break;

		case Tip_DoResearch3:
			str = "You should start research on a\n"
				  "technology now if you have not\n"
				  "already. Toggle to the Research screen\n"
				  "by pressing \x88, then double tap on a\n"
				  "technology that's red to start\n"
				  "research. Mining is a good first\n"
				  "choice.\n";
			break;

		case Tip_CompletedFirstResearch:
			str = "Good! You've completed research on\n"
				  "your first technology. You should\n"
				  "continue researching technologies\n"
				  "to gain their benefits, balancing\n"
				  "research against new construction.\n"
				  "\n"
				  "You'll learn the benefit of each\n"
				  "technology when it completes by the\n"
				  "message display at the top of the\n";
			multi = true;
			mHaveDisplayed[Tip_DoResearch] = true;
			Add(Tip_CompletedFirstResearch2);
			break;

		case Tip_CompletedFirstResearch2:
			str = "top screen.\n"
				  "\n"
				  "Over time, you'll develop a sense\n"
				  "for the technologies that complement\n"
				  "your playing style, and when they\n"
				  "should be researched for maximum\n"
				  "effectiveness.";
			break;

		case Tip_AboutTornados:
			str = "Occasionally, a tornado will whip\n"
				  "across your island. Tornados destroy\n"
				  "most things in their path, however\n"
				  "the faster the tornado is moving\n"
				  "the better chance your buildings\n"
				  "have of surviving. There's not much\n"
				  "you can do about tornados, except\n"
				  "rebuild as quickly as possible after\n"
				  "the tornado has passed.";
			break;

		case Tip_TechUp:
			str = "Your island is starting to fill up.\n"
				  "In order to grow your population\n"
				  "even larger, you'll need to research\n"
				  "technologies that allow greater\n"
				  "density of food and housing. Farming\n"
				  "and Construction allow you to build\n"
				  "Farms and Houses which support many\n"
				  "more people than Crops and Huts.\n";
			break;

		case Tip_OnlyOneBuilding:
			str = "You can only build one building at a\n"
				  "time. If you start a building when\n"
				  "you're already building another\n"
				  "building all progress will be lost on\n"
				  "the first and production will start\n"
				  "on the new one.";
			break;

		case Tip_School:
			str = "Once your population has reached\n"
				  "a certain level, you can no longer\n"
				  "keep them as uneducated masses: they\n"
				  "will demand a School.\n"
				  "\n"
				  "You can also satisfy their education\n"
				  "needs by building a University once\n"
				  "you have an even larger population.\n";
			break;

		case Tip_BulldozerBeforeBuilding:
			str = "If you want to replace a building\n"
				  "with a new one, you must first\n"
				  "destroy the old one.\n"
				  "\n"
				  "To do this, choose Destroy from the \x87\n"
				  "menu, and tap the building you want\n"
				  "to destroy. Then, you can build the\n"
				  "new building.";
			break;

		default:
			assert(false && "unhandled tip");
			return;
	}

	mFont.DrawString(bg, str, 15, 15, 0x8000 | RGB15(31, 31, 31), TextAlignment_Left);
	if (multi)
	{
		mFont.DrawString(bg, "(more...)", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 30, 0x8000 | RGB15(25, 25, 25), TextAlignment_Centre);
	}
	else
	{
		mFont.DrawString(bg, "Tap to dismiss, \x85 Disable tips", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 30, 0x8000 | RGB15(25, 25, 25), TextAlignment_Centre);
	}
}

bool Tips::Update(u16* bg, u16 down)
{
	bool ret = false;
	if (!mGloballyEnabled) return ret;

	if (mDisplaying == Tip_Invalid)
	{
		for (int i = Tip_First; i < Tip_Count; ++i)
		{
			if (mPending[i])
			{
				mDisplaying = (TipType)i;
				mPending[i] = false;
				mHaveDisplayed[i] = true;
				Draw(bg);
				Play(Sound_Balloon);
				ret = true;
				break;
			}
		}
	}

	if (IsDisplaying())
	{
		if (down & KEY_X)
		{
			mGloballyEnabled = false;
			mDisplaying = Tip_Invalid;
			Clear(bg, 0);
		}
	}

	return ret;
}

void Tips::Dismiss(u16* bg)
{
	mDisplaying = Tip_Invalid;
	Clear(bg, 0);
}

void Tips::ClearAllPending()
{
	memset(mPending, 0, sizeof(mPending));
}
