#ifndef LG_INCLUDED_linegraph_H
#define LG_INCLUDED_linegraph_H

#include "lg/f32.h"
namespace luvafair { class Font; }

void DrawLineGraph(u16* bg, luvafair::Font& font, const luvafair::F32* history, u16 numElems, const char* title);

#endif
