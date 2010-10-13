#ifndef INCLUDED_iconmenu_H
#define INCLUDED_iconmenu_H

#include "buildings.h"
#include "lg/v2.h"

namespace luvafair { class Font; }

class IconMenu
{
	luvafair::Font& mFont;
	int mColumns;
	enum { MAX_ICONS = 16 };
	BuildingIndex mIcons[MAX_ICONS];
	const char* mCaptions[MAX_ICONS];
	bool mEnabled[MAX_ICONS];
	int mNumIcons;
	const u16* mPalette;
	const u8* mTileData;
	int mFirstBuildingIndex;

	void GetRect(int& x, int& y, int& w, int& h, int& rows);
	void DrawIcon(u16* bg, int xx, int yy, const u8* icon);

	public:
		IconMenu(luvafair::Font& font, int columns, const u8* mainpal, const u8* tileData, int firstBuildingIndex);
		int Add(BuildingIndex index, const char* caption = "");
		void SetEnabled(BuildingIndex index, bool enabled);
		void Draw(u16* bg);
		void Erase(u16* bg);
		BuildingIndex Tap(const V2& position);
};

#endif
