#ifndef INCLUDED_edge_arrows_H
#define INCLUDED_edge_arrows_H

#include "lg/sprite.h"

class EdgeArrows
{
	luvafair::SpriteData arrow;
	luvafair::SpriteData buildart, researchart;
	luvafair::SpriteInstance right, down, left, up, build, research;
	luvafair::SpriteMatrix plus90, minus90;

	public:
		EdgeArrows();
		void ShowLeft(bool on);
		void ShowRight(bool on);
		void ShowUp(bool on);
		void ShowDown(bool on);
};


#endif
