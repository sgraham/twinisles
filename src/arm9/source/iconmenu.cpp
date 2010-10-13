#include "lg/lg.h"
#include "iconmenu.h"
#include "lg/draw.h"
#include "lg/font.h"
#include "memory_config.h"
#include "sound_effects.h"

#define LG_ICONSIZE LG_ART_SQUARE_SIZE
#define LG_SPACING 12
#define LG_FROM_SIDE 12

using namespace luvafair;

IconMenu::IconMenu(Font& font, int columns, const u8* mainpal, const u8* tileData, int firstBuildingIndex)
	: mFont(font)
	, mColumns(columns)
	, mNumIcons(0)
	, mPalette((const u16*)mainpal)
	, mTileData(tileData)
	, mFirstBuildingIndex(firstBuildingIndex)
{
}

int IconMenu::Add(BuildingIndex index, const char* caption)
{
	assert(mNumIcons < MAX_ICONS);
	mIcons[mNumIcons] = index;
	mCaptions[mNumIcons] = caption;
	mEnabled[mNumIcons] = true;
	return mNumIcons++;
}

void IconMenu::GetRect(int& x, int& y, int& w, int& h, int& rows)
{
	rows = (mNumIcons + mColumns - 1) / mColumns;
	h = LG_ICONSIZE * rows + LG_SPACING * (rows + 1);
	w = LG_ICONSIZE * mColumns + LG_SPACING * (mColumns + 1);
	x = SCREEN_WIDTH - w - LG_FROM_SIDE;
	y = LG_FROM_SIDE;
}

void IconMenu::DrawIcon(u16* bg, int xx, int yy, const u8* icon)
{
	// we're using the tiled data from the map here, so the drawing is kind of complicated
	// probably should have just made a second unswizzled copy instead
	for (int y = 0; y < LG_ICONSIZE/8; ++y)
	{
		for (int x = 0; x < LG_ICONSIZE/8; ++x)
		{
			for (int ty = 0; ty < 8; ++ty)
			{
				for (int tx = 0; tx < 8; ++tx)
				{
					bg[(yy + (y*8) + ty) * 256 + (xx + (x*8) + tx)] = mPalette[icon[y * 64 * 4 + x * 64 + ty * 8 + tx]];
				}
			}
		}
	}
}

void IconMenu::Draw(u16* bg)
{
	int x, y, w, h, rows;
	GetRect(x, y, w, h, rows);
	luvafair::DrawFilledRect(bg, x, y, w, h, 0x8000 | RGB15(10, 10, 10));
	int i = 0;
	for (int c = 0; c < mColumns; ++c)
	{
		for (int r = 0; r < rows; ++r)
		{
			int xpos = x + LG_SPACING + c * (LG_SPACING + LG_ICONSIZE);
			int ypos = y + LG_SPACING + r * (LG_SPACING + LG_ICONSIZE);
			// assuming tile data is 8bit
			DrawIcon(bg, xpos, ypos, &mTileData[(mFirstBuildingIndex + mIcons[i]) * LG_ICONSIZE * LG_ICONSIZE]);
			u16 fontclr = 0x8000 | RGB15(31,31,31);
			if (!mEnabled[i]) fontclr = 0x8000 | RGB15(18, 18, 18);
			mFont.DrawString(bg, mCaptions[i], xpos + LG_ICONSIZE / 2, ypos + LG_ICONSIZE - 7, fontclr, TextAlignment_Centre);
			++i;
			if (i == mNumIcons) return;
		}
	}
}

void IconMenu::Erase(u16* bg)
{
	int x, y, w, h, rows;
	GetRect(x, y, w, h, rows);
	luvafair::DrawFilledRect(bg, x, y, w, h, 0);
}

void IconMenu::SetEnabled(BuildingIndex index, bool enabled)
{
	for (int i = 0; i < mNumIcons; ++i)
	{
		if (mIcons[i] == index)
		{
			mEnabled[i] = enabled;
			return;
		}
	}
}

BuildingIndex IconMenu::Tap(const V2& position)
{
	int x, y, w, h, rows;
	GetRect(x, y, w, h, rows);
	int i = 0;
	for (int c = 0; c < mColumns; ++c)
	{
		for (int r = 0; r < rows; ++r)
		{
			int xpos = x + LG_SPACING + c * (LG_SPACING + LG_ICONSIZE);
			int ypos = y + LG_SPACING + r * (LG_SPACING + LG_ICONSIZE);
			if (position.x > xpos && position.x < xpos + LG_ICONSIZE
					&& position.y > ypos && position.y < ypos + LG_ICONSIZE
					&& mEnabled[i])
			{
				Play(Sound_Click);
				return mIcons[i];
			}
			++i;
			if (i == mNumIcons)
			{
				Play(Sound_Error);
				return BuildingIndex_Invalid;
			}
		}
	}

	assert(false);
	return BuildingIndex_Invalid;
}
