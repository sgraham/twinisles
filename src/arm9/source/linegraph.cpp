#include "linegraph.h"
#include "lg/draw.h"
#include "lg/font.h"

using namespace luvafair;

void DrawLineGraph(u16* bg, luvafair::Font& font, const F32* history, u16 numElems, const char* title)
{
	const int BORDER = 60;
	const int W = SCREEN_WIDTH - BORDER * 2;
	const int BOX_H = SCREEN_HEIGHT - BORDER * 2;
	const int GRAPH_H = SCREEN_HEIGHT - BORDER * 2 - 20;
	DrawFilledRect(bg, BORDER, BORDER, W, BOX_H + 1, 0x8000 | RGB15(31, 31, 31)); // todo; +1
	font.DrawString(bg, title, SCREEN_WIDTH / 2, BORDER + 5, 0x8000 | RGB15(0,0,0), TextAlignment_Centre);

	if (numElems == 0) return;


	F32 largest = 0;
	for (int i = 0; i < numElems; ++i)
	{
		if (history[i] > largest)
		{
			largest = history[i];
		}
	}
	largest *= F32(1.1f);

	char buf[128];
	sprintf(buf, "%d", largest.Int());
	font.DrawString(bg, buf, 0, BORDER + 13);
	font.DrawString(bg, "0", 0, SCREEN_HEIGHT - BORDER - 7);

	F32 x(BORDER);
	F32 xstep(F32(W) / F32(numElems));
	F32 y(F32(GRAPH_H) - F32(history[0]) / largest * F32(GRAPH_H));
	for (int i = 1; i < numElems; ++i)
	{
		F32 x2(x + xstep);
		F32 y2(F32(GRAPH_H) - F32(history[i]) / largest * F32(GRAPH_H));
		DrawLine(bg, x.Int(), y.Int() + BORDER + 20, x2.Int(), y2.Int() + BORDER + 20, 0x8000 | RGB15(31, 0, 0));
		x = x2;
		y = y2;
	}
	sprintf(buf, "%d", history[numElems - 1].Int());
	font.DrawString(bg, buf, SCREEN_WIDTH, y.Int() + BORDER + 20 - 7, 0x8000 | RGB15(31, 31, 31), TextAlignment_Right);
}
