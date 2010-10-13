#include "lg/lg.h"
#include "gui.h"
#include "font_noaa_bin.h"
#include "lg/draw.h"
#include "sound_effects.h"

#define Colour_ButtonBorder (0x8000 | RGB15(15, 15, 20))
#define Colour_ButtonInlay (0x8000 | RGB15(10, 10, 15))
#define Colour_TextEnabled (0x8000 | RGB15(31, 31, 31))
#define Colour_TextDisabled (0x8000 | RGB15(15, 15, 15))

typedef void (*ButtonCallback)(GuiItem* item, Screen* screen, void* p);

using namespace luvafair;

void Button::Draw(u16* bg)
{
	Font font(font_noaa_bin);

	DrawFilledRect(bg, x, y, w, h, Colour_ButtonBorder);
	DrawFilledRect(bg, x + 2, y + 2, w - 4, h - 4, Colour_ButtonInlay);
	font.DrawString(bg, mText, x + w / 2, y + h / 2 - font.GetHeight() / 2,
			mCallback ? Colour_TextEnabled : Colour_TextDisabled,
			TextAlignment_Centre);
}

void Button::SetCallback(ButtonCallback func)
{
	mCallback = func;
}

void Button::SetText(const char* text)
{
	mText = text;
}

bool Button::TapTest(const V2& p, Screen* screen)
{
	if (mCallback == 0) return false;

	if (p.x > x && p.y > y
			&& p.x <= x + w && p.y <= y + h)
	{
		mCallback(this, screen, mUser);
		Play(Sound_Click);
		return true;
	}
	return false;
}


void Label::Draw(u16* bg)
{
	Font font(font_noaa_bin);
	font.DrawString(bg, mText, x, y, mColour, mTA);
}


void Image::Draw(u16* bg)
{
	DrawIcon(bg, mImage, x, y, w, h);
}


void Background::Draw(u16* bg)
{
	Clear(bg, 0x8000 | mColour);
}

void Arrow::Draw(u16* bg)
{
	if (mType == Arrow_Left)
	{
	}
	else if (mType == Arrow_Right)
	{
		const int points[] =
		{
			x + w/2, y + 0,
			x + w-1, y + h/2,
			x + w/2, y + h-1,
			x + w/2, y + 2*h/3,
			x,       y + 2*h/3,
			x,       y + h/3,
			x + w/2, y + h/3,
		};
		DrawPolygon(bg, ARRAYSIZE(points) / 2, points, Colour_ButtonBorder);
	}
}

bool Arrow::TapTest(const V2& p, Screen* screen)
{
	if (mCallback == 0) return false;

	if (p.x > x && p.y > y
			&& p.x <= x + w && p.y <= y + h)
	{
		mCallback(this, screen, mUser);
		Play(Sound_Click);
		return true;
	}
	return false;
}

void WrappedLabel::Draw(u16* bg)
{
	Font font(font_noaa_bin);
	font.DrawStringWrapped(bg, mText, x, y, w, mColour, TextAlignment_Left);
}

void WrappedLabel::SetText(const char* text)
{
	mText = text;
}

void Screen::Add(GuiItem* item)
{
	assert(mNumItems < ARRAYSIZE(mItems));
	mItems[mNumItems++] = item;
}

void Screen::Tap(const V2& p)
{
	for (int i = mNumItems - 1; i >= 0; --i)
	{
		if (mItems[i]->TapTest(p, this))
		{
			break;
		}
	}
}

void Screen::Draw()
{
	for (int i = 0; i < mNumItems; ++i)
	{
		mItems[i]->Draw(mBG);
	}
}

Screen::~Screen()
{
	for (int i = 0; i < mNumItems; ++i)
	{
		delete mItems[i];
	}
}
