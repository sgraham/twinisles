#include "lg/lg.h"
#include "edge_arrows.h"
#include "mainpal_bin.h"
#include "arrow_bin.h"
#include "hud_build_bin.h"
#include "hud_research_bin.h"

using namespace luvafair;

EdgeArrows::EdgeArrows()
	: arrow(16, 16, mainpal_bin)
	, buildart(64, 32, mainpal_bin)
	, researchart(64, 32, mainpal_bin)
	, right(&arrow)
	, down(&arrow)
	, left(&arrow)
	, up(&arrow)
	, build(&buildart)
	, research(&researchart)
{
	arrow.AddFrame(arrow_bin);
	
	buildart.AddFrame(hud_build_bin);
	researchart.AddFrame(hud_research_bin);

	minus90.SetRotation(270);
	down.SetMatrix(&minus90);

	SpriteInstance arrowUp(&arrow);
	SpriteMatrix plus90rot;
	plus90.SetRotation(90);
	up.SetMatrix(&plus90);

	SpriteInstance arrowLeft(&arrow);
	left.SetFlipX(true);

	ShowLeft(false); ShowUp(false);
	ShowRight(true); ShowDown(true);

	build.SetPosition(0, 0);
	research.SetPosition(SCREEN_WIDTH-64, 0);
}

void EdgeArrows::ShowLeft(bool on)
{
	if (on)
		left.SetPosition(0, 192/2-16/2);
	else
		left.SetPosition(256, 192);
}

void EdgeArrows::ShowRight(bool on)
{
	if (on)
		right.SetPosition(256-16, 192/2-16/2);
	else
		right.SetPosition(256, 192);
}

void EdgeArrows::ShowUp(bool on)
{
	if (on)
		up.SetPosition(256/2-16/2, 0);
	else
		up.SetPosition(256, 192);
}

void EdgeArrows::ShowDown(bool on)
{
	if (on)
		down.SetPosition(256/2-16/2, 192-16);
	else
		down.SetPosition(256, 192);
}
