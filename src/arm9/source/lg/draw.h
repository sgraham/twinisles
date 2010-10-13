#ifndef LG_INCLUDED_lg_draw_H
#define LG_INCLUDED_lg_draw_H

namespace luvafair
{
	void Clear(u16* bg, u16 colour);
	void DrawFilledRect(u16* bg, int x, int y, int w, int h, u16 colour);
	void DrawIcon(u16* bg, u16* icon, int x, int y, int w, int h);
	void DrawLine(u16* bg, int x0, int y0, int x1, int y1, u16 colour);
	void DrawPolygon(u16* bg, int numPoints, const int* points, u16 colour);
};

#endif
