#ifndef LG_INCLUDED_lg_font_H
#define LG_INCLUDED_lg_font_H

#include "lg/lg.h"

namespace luvafair
{
	enum FontButton
	{
		FontButton_Right = 127,
		FontButton_Left = 128,
		FontButton_Up = 129,
		FontButton_Down = 130,
		FontButton_A = 131,
		FontButton_B = 132,
		FontButton_X = 133,
		FontButton_Y = 134,
		FontButton_L = 135,
		FontButton_R = 136,
		FontButton_Dpad = 137,

		FontButton_LastButton = FontButton_Dpad
	};

	enum TextAlignment
	{
		TextAlignment_Left = 0,
		TextAlignment_Right = 1,
		TextAlignment_Centre = 2,

	};

	enum { FontFirstCharacter = ' ' };

	struct __attribute__((packed)) FontData
	{
		u16 width;
		u16 height;
		u16 xPositions[FontButton_LastButton - FontFirstCharacter + 1];
		u8 charWidths[FontButton_LastButton - FontFirstCharacter + 1];
	};

	class Font
	{
		const FontData* mFontData;

		void AdjustXForAlignment(const char* str, int& sx, TextAlignment alignment);

		public:
			Font(const u8* fontData);
			void DrawString(u16* bg, const char* str, int x, int y, u16 colour = 0x8000 | RGB15(31, 31, 31), TextAlignment alignment = TextAlignment_Left);
			void DrawStringWrapped(u16* bg, const char* str, int x, int y, int w, u16 colour = 0x8000 | RGB15(31, 31, 31), TextAlignment alignment = TextAlignment_Left);
			void DrawString8(u16* bg, const char* str, int x, int y, u8 colour, TextAlignment alignment = TextAlignment_Left);

			int GetHeight() const { return mFontData->height; }
			int GetWidth(const char* str);

	};
#if 0
	class Font
	{
		const FontData* mMetricsData;
		const Texture* mTexture;
		public:
			Font(const u8* metricsData, const Texture* texture);
			void DrawString(const char* str, int x, int y, u16 colour = 0x8000 | RGB15(31, 31, 31), TextAlignment alignment = TextAlignment_Left);
	};
#endif
}

#endif
