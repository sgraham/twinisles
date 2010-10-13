#include "lg/lg.h"
#include "lg/draw.h"
#include "lg/srast.h"

namespace luvafair
{
	void DrawFilledRect(u16* bg, int xx, int yy, int w, int h, u16 colour)
	{
		assert(w % 2 == 0);
		u32 colcol = colour | (colour << 16);
		for (int y = yy; y < h + yy; ++y)
		{
			swiFastCopy(&colcol, &bg[y * 256 + xx], w * 2 / 4 | COPY_MODE_FILL);
		}
	}

	void Clear(u16* bg, u16 colour)
	{
		u32 colcol = colour | (colour << 16);
		swiFastCopy(&colcol, bg, 256*192*2/4 | COPY_MODE_FILL);
	}

	void DrawIcon(u16* bg, u16* icon, int x, int y, int w, int h)
	{
		for (int yy = 0; yy < h; ++yy)
		{
			for (int xx = 0; xx < w; ++xx)
			{
				bg[(yy + y) * 256 + (xx + x)] = icon[yy*w+xx];
			}
		}
	}

	void DrawLine(u16* bg, int x0, int y0, int x1, int y1, u16 colour)
	{
		SRastBuffer<u16> buf(bg, 256, 256, 192);
		buf.DrawLine(x0, y0, x1, y1, colour);
	}

	void DrawPolygon(u16* bg, int numPoints, const int* points, u16 colour)
	{
		SRastBuffer<u16> buf(bg, 256, 256, 192);
		buf.DrawPolygon(numPoints, points, colour);
	}
}
