#include "lg/lg.h"
#include "lg/font.h"
#include <cstdio>
#include <cctype>

namespace luvafair
{

#if 0
Font::Font(const u8* metricsData, const Texture* texture)
	: mMetricsData((const FontData*)metricsData)
	, mTexture(texture)
{
}
#endif

Font::Font(const u8* fontData)
	: mFontData((const FontData*)fontData)
{
}

int Font::GetWidth(const char* str)
{
	int w = 0;

	const char *tmp = str;
	for (;;)
	{
		unsigned char c = *tmp++;
		if (c == 0) break;
		assert(c != '\n');
		if (c < FontFirstCharacter || c > FontButton_LastButton) continue;

		w += mFontData->charWidths[c - FontFirstCharacter];
	}

	return w;
}

void Font::AdjustXForAlignment(const char* str, int& sx, TextAlignment alignment)
{
	assert(str);

	if (alignment == TextAlignment_Centre || alignment == TextAlignment_Right)
	{
		int w = GetWidth(str);

		if (alignment == TextAlignment_Right)
		{
			sx -= w;
		}
		else //if (alignment == TextAlignment_Centre)
		{
			sx -= w/2;
		}
	}
}

static const char* SkipSpaces(const char* t)
{
	for (;;)
	{
		if (!isspace(*t)) return t;
		++t;
	}
}

static void WrapText(int width, Font& font, const char* inText, char* outText)
{
	const char* s = inText;
	char* d = outText;
	char* startOfLine = outText;
	const char *startOfWordSource = SkipSpaces(inText);
	char *lastFullWordDest = d;

	for (;;)
	{
		char c = *s;
		if (c == 0)
		{
			*d = 0;
			break;
		}
		else if (c == '\n')
		{
			d[0] = '\n';
			d[1] = 0;
			startOfLine = &d[1];
			++s;
			++d;
		}
		else
		{
			if (isspace(c))
			{
				startOfWordSource = s;
				lastFullWordDest = d;
			}
			d[0] = c;
			d[1] = 0;
			if (font.GetWidth(startOfLine) > width)
			{
				d = lastFullWordDest;
				d[0] = '\n';
				d[1] = c;
				s = SkipSpaces(startOfWordSource);
				startOfLine = &d[1];
				++d;
			}
			else
			{
				++s;
				++d;
			}
		}
	}
}

void Font::DrawStringWrapped(u16* bg, const char* str, int x, int y, int w, u16 colour, TextAlignment alignment)
{
	char buf[1024]; // lame-o
	WrapText(w, *this, str, buf);
	DrawString(bg, buf, x, y, colour, alignment);
}

void Font::DrawString(u16* bg, const char* str, int sx, int sy, u16 colour, TextAlignment alignment)
{
	AdjustXForAlignment(str, sx, alignment);
	const u8* charData = (const u8*)(mFontData + 1);
	int origx = sx;
	for (;;)
	{
		unsigned char c = *str++;
		if (c == 0) break;
		if (c == '\n')
		{
			assert(alignment == TextAlignment_Left && "haven't done multiple lines for non-left-aligned text");
			sy += mFontData->height;
			sx = origx;
		}
		if (c < FontFirstCharacter || c > FontButton_LastButton) continue;

		u16 xOffset = mFontData->xPositions[c - FontFirstCharacter];
		for (int y = 0; y < mFontData->height; ++y)
		{
			if ((sy + y) < 0) continue;
			if ((sy + y) >= SCREEN_HEIGHT) break;
			for (int x = 0; x < mFontData->charWidths[c - FontFirstCharacter]; ++x)
			{
				if ((sx + x) < 0) continue;
				if ((sx + x) >= SCREEN_WIDTH) break;

				u8 data = charData[y * mFontData->width + xOffset + x];
				//iprintf("%02x, ", data);
				data >>= 3;
				u16 clr = 0;
				if (data > 3)
				{
					u16 r = (colour & 0x1f) * data / 31;
					u16 g = (((colour & (0x1f << 5))) >> 5) * data / 31;
					u16 b = (((colour & (0x1f << 10)) >> 10)) * data / 31;
					clr = 0x8000 | RGB15(r, g, b);
					bg[(sy + y) * 256 + sx + x] = clr;
				}
			}
			//iprintf("\n");
		}
		sx += mFontData->charWidths[c - FontFirstCharacter];
	}
}

void Font::DrawString8(u16* bg, const char* str, int sx, int sy, u8 colour, TextAlignment alignment)
{
	AdjustXForAlignment(str, sx, alignment);
	const u8* charData = (const u8*)(mFontData + 1);
	u16 colourHi = colour << 8;
	u16 colourLo = colour;
	int origx = sx;
	for (;;)
	{
		unsigned char c = *str++;
		if (c == 0) break;
		if (c == '\n')
		{
			assert(alignment == TextAlignment_Left && "haven't done multiple lines for non-left-aligned text");
			sy += mFontData->height;
			sx = origx;
		}
		if (c < FontFirstCharacter || c > FontButton_LastButton) continue;

		u16 xOffset = mFontData->xPositions[c - FontFirstCharacter];
		for (int y = 0; y < mFontData->height; ++y)
		{
			for (int x = 0; x < mFontData->charWidths[c - FontFirstCharacter]; ++x)
			{
				u8 data = charData[y * mFontData->width + xOffset + x];
				//iprintf("%02x, ", data);
				data >>= 3;
				if (data > 3)
				{
					// todo; this is abysmally slow
					u16 read = bg[(sy + y) * 256 + (sx + x) / 2];
					if ((sx + x) % 2 == 1)
					{
						read &= 0xff;
						bg[(sy + y) * 256 + (sx + x) / 2] = read | colourHi;
					}
					else
					{
						read &= 0xff00;
						bg[(sy + y) * 256 + (sx + x) / 2] = read | colourLo;
					}
				}
			}
			//iprintf("\n");
		}
		sx += mFontData->charWidths[c - FontFirstCharacter];
	}
}

}
