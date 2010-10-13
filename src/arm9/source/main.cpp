/*
 
todo; 
start both players at centre of their island
wifi

// bugs:
// slider btwn research/building, or pause research?
// new level? multiple levels?
// crops expired sound
// other bonuses for near/not water (apt hold more near water?)
// Pressed vs/ PressedSolid
// fishing, trade, war boats and their mechanics
// factory lowers birthrate?
// make sure all techs actually do something moderately useful
// fossil fuels too expensive?
// tornado doesn't destroy in centre
// fishing boats, wander randomly
// tornado resistance research item
// food import blimp mechanics (fly off in random direction, come back after a while)
// make sure replays still work with cpu, etc.


*/
#include <nds.h>
#include <cstdio>
#include <cstring>
#include <nds/arm9/sound.h>
#include <fat.h>
#include "mainpal_bin.h"
#include "default_bin.h"
#include "tiles_bin.h"
#include "arrow_bin.h"
#include "test_bin.h"
#include "font_noaa_bin.h"
#include "buildings.h"
#include "lg/lg.h"
#include "lg/sprite.h"
#include "lg/font.h"
#include "lg/texture.h"
#include "edge_arrows.h"
#include "object_mgr.h"
#include "render.h"
#include "map.h"
#include "sim.h"
#include "memory_config.h"
#include "lg/v2.h"
#include "lg/draw.h"
#include "iconmenu.h"
#include "message.h"
#include "tips.h"
#include "hand_bin.h"
#include "progress_build.h"
#include "techtree_bin.h"
#include "techtree.h"
#include "vsad_bin.h"
#include "sad_bin.h"
#include "moderate_bin.h"
#include "happy_bin.h"
#include "vhappy_bin.h"
#include "sound_effects.h"
#include "touch_smoother.h"
#include "fe.h"
#include "audio.h"
#include "gui.h"
#include "saved_inputs.h"
#include "lg/string.h"
#include "scenario.h"
#include "cpu_player.h"
#include "util.h"
#include "lg/scoped_ptr.h"
#include "lg/rand.h"

using namespace luvafair;

void SwapToTechTree(bool isTechTree)
{
	u16 cr = BG_PRIORITY(0)							// bit 0-1, prority
		| 0											// bit 6, mosaic enable
		| BIT(7)									// bit 7, exrot (1 == bitmap)
		| BG_BMP_BASE(LG_OVERLAY_LAYER_BASE_SUB);	// bit 8-12, screen base block
	if (isTechTree)
	{
		cr |= BIT(15); 								// bit 14-15, screen size (10 == 512x256)
	}
	else
	{
		cr |= BIT(14)								// bit 14-15, screen size (01 == 256x256)
			| BIT(2);								// 16 bit colour mode
	}
	SUB_BG3_CR = cr;
	SUB_BG3_XDX = 1<<8;
	SUB_BG3_XDY = 0;
	SUB_BG3_YDX = 0;
	SUB_BG3_YDY = 1<<8;
	SUB_BG3_CY = 0;
	SUB_BG3_CX = 0;

#define LG_TECHTREE_COLOUR_UNAVAILABLE 10
#define LG_TECHTREE_COLOUR_RESEARCHED 31
#define LG_TECHTREE_COLOUR_AVAILABLE_TO_RESEARCH 32
#define LG_TECHTREE_COLOUR_RESEARCHING 33
#define LG_TECHTREE_COLOUR_BLACK 31
	for (int i = 0; i < 31; ++i)
	{
		BG_PALETTE_SUB[i] = RGB15(31-i, 31-i, 31-i);
	}

	if (isTechTree)
	{
		swiFastCopy((void*)techtree_bin, (void*)BG_BMP_RAM_SUB(LG_OVERLAY_LAYER_BASE_SUB), 512*256/4);
	}
}

void MainHardwareInit()
{
	powerON(POWER_ALL);

	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D | DISPLAY_SPR_1D_SIZE_256);
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);

	irqInit();
	irqEnable(IRQ_VBLANK);
	
	vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000, VRAM_C_SUB_BG_0x06200000, VRAM_D_TEXTURE);
	vramSetBankE(VRAM_E_MAIN_SPRITE);
	lcdSwap();

	// holy crap, the modes and options on this mother are crazy. these registers set up two backgrounds:
	// mode5 (above) means that bg2 and bg3 are extended rotation backgrounds
	// for bg2, bit 7 and bizarrely bit 2 being 0 means tiles/map version
	// for bg3, bit 7 is set, and bit 2 is unset, meaning 8bit bitmap
	// bit7 and bit2 both 1 means direct color bitmap ABGR
	// decided against bg3 for now, using the 3d hardware for font overlays to get nicer AA instead (bg0)
	// actually, not doing 3d because dualis can't handle it so we live with non-aa'd text for now, oh well

	assert(LG_BACKGROUND_TILES_SIZE == 128 && "BG2_CR needs to be updated");
	BG2_CR = BG_PRIORITY(2)							// bit 0-1, prority
		| BG_TILE_BASE(LG_TILES_OFFSET)				// bit 2-5, character base block, n * 16k
		| 0											// bit 6, mosaic enable
		| 0											// bit 7, ex rot bg style 00
		| BG_MAP_BASE(LG_MAP_OFFSET)				// bit 8-12, screen base block (n * 2k)
		| BG_RS_128x128;							// bit 14-15, screen size
	BG2_XDX = 1<<8;
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = 1<<8;
	BG2_CY = 0;
	BG2_CX = 0;

	BG3_CR = BG_PRIORITY(0)							// bit 0-1, prority
		| BIT(2)									// bit 2-5, set to 0 for 16 bit argb
		| 0											// bit 6, mosaic enable
		| BIT(7)									// bit 7, exrot (1 == bitmap)
		| BG_BMP_BASE(LG_OVERLAY_LAYER_BASE)		// bit 8-12, screen base block
		| BIT(14);									// bit 14-15, screen size (01 == 256x256)
	BG3_XDX = 1<<8;
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = 1<<8;
	BG3_CY = 0;
	BG3_CX = 0;

	SwapToTechTree(false);

	fatInitDefault();
}


u16 statsOffscreenBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];

static u16* GetHappinessIcon(F32 happy)
{
	if (happy < F32(.3f)) return (u16*)vsad_bin;
	else if (happy < F32(.7f)) return (u16*)sad_bin;
	else if (happy < F32(1.1f)) return (u16*)moderate_bin;
	else if (happy < F32(1.4f)) return (u16*)happy_bin;
	else return (u16*)vhappy_bin;
}

void DrawStatus(u16* bg, Font& font, const Sim& sim, BuildingIndex toBuy, GameMode mode)
{
	char buf[128];

	// todo; this could be faster; right justify text and do it once, then just redraw numbers
	F32 food, housing, pop, prod, happy, prodRB, prodBB, prodUpkeep;
	sim.GetCurrentValues(PlayerId(0), &food, &housing, &pop, &prod, &happy, &prodRB, &prodBB, &prodUpkeep);

	sprintf(buf, "Food support: %d", food.Int());
	font.DrawString(bg, buf, 10, 40, 0x8000 | RGB15(20, 20, 20));

	sprintf(buf, "Housing support: %d", housing.Int());
	font.DrawString(bg, buf, 10, 60, 0x8000 | RGB15(20, 20, 20));

	sprintf(buf, "Current population: %d", pop.Int());
	font.DrawString(bg, buf, 10, 80, 0x8000 | RGB15(20, 20, 20));

	char extra[128] = {0};
	if (prodRB != 0 || prodBB != 0 || prodUpkeep != 0)
	{
		strcpy(extra, "(");
		char temp[128];
		if (prodRB != 0)
		{
			sprintf(temp, "+%dR", (prodRB * 100).Int());
			strcat(extra, temp);
		}
		if (prodBB != 0)
		{
			sprintf(temp, "+%dB", (prodBB * 100).Int());
			strcat(extra, temp);
		}
		if (prodUpkeep != 0)
		{
			sprintf(temp, "-%dU", (prodUpkeep * 100).Int());
			strcat(extra, temp);
		}
		strcat(extra, ")");
	}
	sprintf(buf, "Production: %d %s", (prod * 100).Int(), extra);
	font.DrawString(bg, buf, 10, 100, 0x8000 | RGB15(20, 20, 20));

	DrawIcon(bg, GetHappinessIcon(happy), 180, 60, 32, 32);

	bool kfood, khousing, kschool;
	sim.GetKilledOff(PlayerId(0), &kfood, &khousing, &kschool);
	sprintf(buf, "%s\n%s\n%s", kfood ? "F" : "", khousing ? "H" : "", kschool ? "S" : "");
	font.DrawString(bg, buf, 220, 50, 0x8000 | RGB15(20, 20, 20));


	if (toBuy != BuildingIndex_Invalid)
	{
		if (toBuy == BuildingIndex_Bulldozer)
		{
			strcpy(buf, "Tap to destroy");
		}
		else if (toBuy == BuildingIndex_ReclaimedLand)
		{
			strcpy(buf, "Tap to start reclaiming land");
		}
		else
		{
			sprintf(buf, "Tap to start building %s", NameOf(toBuy));
		}
		font.DrawString(bg, buf, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 20, 0x8000 | RGB15(31, 31, 31), TextAlignment_Centre);
	}

	if (mode == GameMode_PlaySavedInputs)
	{
		font.DrawString(bg, "Playing replay", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 20, 0x8000 | RGB15(31, 31, 31), TextAlignment_Centre);
	}
	else if (mode == GameMode_Scenario)
	{
		gScenario->Render(bg, font, sim);
	}

	if (sim.CurrentBuilding(PlayerId(0)) != BuildingIndex_Invalid)
	{
		luvafair::F32 percent = sim.GetPercentageOfBuildingBuilt(PlayerId(0));
		if (sim.CurrentBuilding(PlayerId(0)) == BuildingIndex_ReclaimedLand)
		{
			sprintf(buf, "Reclaming land: %d%%", percent.Int());
		}
		else
		{
			sprintf(buf, "Building %s: %d%%", NameOf(sim.CurrentBuilding(PlayerId(0))), percent.Int());
		}
		font.DrawString(bg, buf, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 40, 0x8000 | RGB15(20, 20, 20), TextAlignment_Centre);
	}
	if (sim.CurrentResearch(PlayerId(0)) != Tech_Invalid)
	{
		char buf[128];
		luvafair::F32 percent = sim.GetPercentageOfResearch(PlayerId(0));
		sprintf(buf, "Researching %s: %d%%", gTechInfo[sim.CurrentResearch(PlayerId(0))].name, percent.Int());
		font.DrawString(bg, buf, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 60, 0x8000 | RGB15(20, 20, 20), TextAlignment_Centre);
	}
}

void RefreshMenu(u16* bg, Sim& sim, IconMenu& menu, PlayerId localPlayer)
{
	Clear(bg, 0);
	for (int i = BuildingIndex_First; i <= BuildingIndex_LastOnBuyMenu; ++i)
	{
		menu.SetEnabled((BuildingIndex)i, sim.CanBuy(localPlayer, (BuildingIndex)i));
	}
	menu.Draw(bg);
}

void DrawTechTree(u16* bg, Sim& sim, Font& font, PlayerId localPlayer)
{
	for (int i = 0; i < Tech_Count; ++i)
	{
		u8 colour = LG_TECHTREE_COLOUR_UNAVAILABLE;
		if (sim.GetHaveTechnology(localPlayer, (Technology)i))
		{
			colour = LG_TECHTREE_COLOUR_RESEARCHED;
		}
		else if (sim.CurrentResearch(localPlayer) == i)
		{
			colour = LG_TECHTREE_COLOUR_RESEARCHING;
		}
		else if (sim.GetHavePrerequisitesForTechnology(localPlayer, (Technology)i))
		{
			colour = LG_TECHTREE_COLOUR_AVAILABLE_TO_RESEARCH;
		}
		font.DrawString8(bg, gTechInfo[i].name,
				gTechInfo[i].screenX,
				gTechInfo[i].screenY - 7,
				colour, TextAlignment_Centre);
	}

	font.DrawString8(bg, "Drag or \x89 to move", 2, 2, LG_TECHTREE_COLOUR_BLACK);
	font.DrawString8(bg, "Double tap to start research", 2, 18, LG_TECHTREE_COLOUR_BLACK, TextAlignment_Left);
}

Technology GetTechTreeBox(const V2& location)
{
	for (int i = 0; i < Tech_Count; ++i)
	{
		const TechnologyInfo* ti = &gTechInfo[i];
		if (location.x > ti->screenX - ti->width / 2
				&& location.x < ti->screenX + ti->width / 2
				&& location.y > ti->screenY - ti->height / 2
				&& location.y < ti->screenY + ti->height / 2)
		{
			return (Technology)i;
		}
	}
	return Tech_Invalid;
}

bool gInPauseMenu;
bool gExitBE;
void ExitBackEnd(GuiItem*, Screen*, void*) { gExitBE = true; }
void ExitPauseMenu(GuiItem*, Screen*, void*)
{
	gInPauseMenu = false;
	u16* bg = (uint16*)BG_BMP_RAM(LG_OVERLAY_LAYER_BASE);
	Clear(bg, 0);
}

void SaveToSRAM(GuiItem* gi, Screen* scr, void* p, bool replay)
{
	SavedInputSaver* saver = (SavedInputSaver*)p;
	Button* button = (Button*)gi;
	saver->SaveComplete();
	if (saver->WriteToSRAM(replay))
	{
		button->SetText("Wrote game");
		gExitBE = true;
	}
	else
	{
		button->SetText("Write failed!");
	}
	scr->Draw();
}

void SaveReplayToSRAMAndQuit(GuiItem* gi, Screen* scr, void* p)
{
	SaveToSRAM(gi, scr, p, true);
}

void SaveGameToSRAMAndQuit(GuiItem* gi, Screen* scr, void* p)
{
	SaveToSRAM(gi, scr, p, false);
}

enum OverlayOwner
{
	OverlayOwner_BuildMenu,
	OverlayOwner_TechTree,
	OverlayOwner_Tips,
	OverlayOwner_PauseMenu,
};

void DoBackEnd(GameMode gm)
{
	EdgeArrows edgeArrows;
	ObjectMgr objects;

	SpriteData hand(16, 16, mainpal_bin);
	hand.AddFrame(hand_bin);
	SpriteInstance handInstance(&hand);
	WorldObject* handCursor = objects.Add(V2(0,0), &handInstance);
	handCursor->Hide();

	//Texture fontTexture(font_texture_bin);
	//Font mainFont(font_metrics_bin, &fontTexture);
	Font mainFont(font_noaa_bin);

	u16* subbg = (uint16*)BG_BMP_RAM_SUB(LG_OVERLAY_LAYER_BASE_SUB);
	u16* bg = (uint16*)BG_BMP_RAM(LG_OVERLAY_LAYER_BASE);
	
	Map map(mainpal_bin, default_bin, tiles_bin, tiles_bin_size);
	Render render;
	MessageDisplay messageDisplay;

	SavedInputPlayer savedInputPlayer;

	Tips tips(mainFont, gm == GameMode_PlaySavedInputs || gm == GameMode_LoadGame);
	SavedInputSaver saver(gm == GameMode_LoadGame ? savedInputPlayer.GetGameMode() : gm,
			gm == GameMode_LoadGame
					? (savedInputPlayer.GetGameMode() == GameMode_Scenario ? savedInputPlayer.GetScenario() : gCurScenario)
					: gCurScenario);
	Rand rand;
	rand.Seed(saver.GetRandSeed());
	Sim sim(map, objects, render, messageDisplay, tips, rand, &saver);
	saver.SetSim(&sim);
	savedInputPlayer.SetSim(&sim);

	if (gm == GameMode_LoadGame)
	{
		rand.Seed(savedInputPlayer.GetRandSeed());
		Clear(bg, 0xffff);
		mainFont.DrawString(bg, "Loading...", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 10, 0x8000 | RGB15(0, 0, 0), TextAlignment_Centre);
		swiWaitForVBlank();

		gCurScenario = savedInputPlayer.GetScenario();
		CreateScenario();

		bool audioIsOn = Audio::SoundIsEnabled();
		if (audioIsOn) Audio::ToggleSoundEnabled();
		for (;;)
		{
			if (savedInputPlayer.Update()) break;
			sim.Update();
		}
		if (audioIsOn) Audio::ToggleSoundEnabled();
		messageDisplay.Clear();

		mainFont.DrawString(bg, "Tap to continue", SCREEN_WIDTH / 2, SCREEN_HEIGHT - 20, 0x8000 | RGB15(0, 0, 0), TextAlignment_Centre);
		swiWaitForVBlank();
		TouchSmoother touch;
		for (;;)
		{
			touch.Update();
			if (touch.Pressed()) break;
			swiWaitForVBlank();
		}

		Clear(bg, 0);
		BLEND_CR = 0;

		// set mode to saved mode
		gm = savedInputPlayer.GetGameMode();
	}
	else if (gm == GameMode_PlaySavedInputs)
	{
		rand.Seed(savedInputPlayer.GetRandSeed());
	}
	else if (gm == GameMode_Scenario)
	{
		CreateScenario();
	}

	BLEND_CR = BLEND_ALPHA | BLEND_SRC_BG3 | BLEND_DST_BG2;

	// todo; network setup stuff here
	PlayerId localPlayer(0);
	PlayerId otherPlayer(1);
	Map::MapFlags localPlayerFlag = Map::Flag_Player1;
	Map::MapFlags localPlayerReclaimFlag = Map::Flag_ReclaimPlayer1;
	Map::MapFlags otherPlayerFlag = Map::Flag_Player2;
	ScopedPtr<CpuPlayer> cpu(0);
	if (gm == GameMode_VsCpu)
	{
		cpu.Reset(new CpuPlayer(PlayerId(1), sim, map, Map::Flag_Player2, Map::Flag_ReclaimPlayer2));
	}
	SoundEffectLocalPlayer = localPlayer;

	IconMenu menu(mainFont, 5, mainpal_bin, tiles_bin, map.FirstBuildingIndex());
	for (int i = BuildingIndex_First; i <= BuildingIndex_LastOnBuyMenu; ++i)
	{
		menu.Add((BuildingIndex)i, NameOfShort((BuildingIndex)i));
	}

	Screen pauseMenu(bg);
	pauseMenu.Add(new Button("Resume", 50, 15, SCREEN_WIDTH - 100, 25, ExitPauseMenu));

	Button* pauseSaveReplayButton = new Button("Save Replay and Quit", 50, 45, SCREEN_WIDTH - 100, 25, SaveReplayToSRAMAndQuit, &saver);
	if (gm == GameMode_PlaySavedInputs) pauseSaveReplayButton->SetCallback(0);
	pauseMenu.Add(pauseSaveReplayButton);

	Button* pauseSaveGameButton = new Button("Save Game and Quit", 50, 75, SCREEN_WIDTH - 100, 25, SaveGameToSRAMAndQuit, &saver);
	if (gm == GameMode_PlaySavedInputs) pauseSaveGameButton->SetCallback(0);
	pauseMenu.Add(pauseSaveGameButton);

	pauseMenu.Add(new Button("Quit", 50, 105, SCREEN_WIDTH - 100, 25, ExitBackEnd));
	luvafair::String scenarioDescription;
	WrappedLabel* scenarioDescLabel = new WrappedLabel(scenarioDescription.c_str(), 10, 140, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 140 - 10, RGB15(31, 31, 31));
	pauseMenu.Add(scenarioDescLabel);

	V2 lastTP;
	V2 scrollLoc(68, 118); // todo; player
	V2 techTreeScrollLoc;
	int menuFade = 0;
	int menuFadeDelta = 0;
	bool isDragScrolling = false;
	BuildingIndex buildingToBuy = BuildingIndex_Invalid;
	int count = 0;
	V2 lastDeltaNormalized;
	F32 lastMoney = 0;
	bool inTechTree = false;
	int techTreePaletteCycle = 0;
	int techTreePaletteCycleDelta = 1;
	int techTreeDoubleTap = 0;
	Technology lastResearch = Tech_Invalid;
	BuildingIndex lastBuildings[2];
	for (int i = 0; i < ARRAYSIZE(lastBuildings); ++i) lastBuildings[i] = BuildingIndex_Invalid;
	gInPauseMenu = false;
	gExitBE = false;
	const char* messageBoxMessage = 0;
	
	ScopedPtr<ProgressBuild> progressBuilds[2];
	for (int i = 0; i < ARRAYSIZE(progressBuilds); ++i)
	{
		progressBuilds[i].Reset(new ProgressBuild(objects));
	}

	TouchSmoother touch;

	Clear(bg, 0);

	while(1)
	{
		touch.Update();

		uint32 down = keysDown();
		uint32 held = keysHeld();
		uint32 up = keysUp();
		V2 touchPos;

		if (down & KEY_LID)
		{
			powerOFF(POWER_ALL);
			if (!gInPauseMenu) down |= KEY_START;
		}
		else if (up & KEY_LID)
		{
			powerON(POWER_ALL);
		}

		if (gm == GameMode_PlaySavedInputs)
		{
			down &= (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN | KEY_START);
			held &= (KEY_LEFT | KEY_RIGHT | KEY_UP | KEY_DOWN | KEY_START);
		}

		if (gInPauseMenu)
		{
			if (down & KEY_START)
			{
				gInPauseMenu = false;
				Clear(bg, 0);
			}
			if (touch.Released())
			{
				pauseMenu.Tap(touch.GetPos());
			}

			if (gExitBE)
			{
				delete gScenario;
				gScenario = 0;
				return;
			}
		}
		else
		{
			if (down & KEY_L)
			{
				if (inTechTree)
				{
					inTechTree = false;
					SwapToTechTree(inTechTree);
					lcdSwap();
				}
				if (!tips.IsDisplaying())
				{
					if (menuFade)
					{
						menuFadeDelta = -1;
						Play(Sound_MenuDown);
					}
					else
					{
						buildingToBuy = BuildingIndex_Invalid;
						RefreshMenu(bg, sim, menu, localPlayer);
						menuFadeDelta = 1;
						Play(Sound_MenuUp);
					}
					down &= ~KEY_L;
				}
			}
			else if (down & KEY_START)
			{
				if (inTechTree)
				{
					inTechTree = false;
					lcdSwap();
					SwapToTechTree(inTechTree);
				}
				if (tips.IsDisplaying())
				{
					tips.ClearAllPending();
					tips.Dismiss(bg);
				}
				if (gm == GameMode_Scenario)
				{
					assert(gScenario);
					scenarioDescription = gScenario->GetDescription(sim);
					scenarioDescLabel->SetText(scenarioDescription.c_str());
				}
				Clear(bg, 0);
				pauseMenu.Draw();
				menuFade = 0;
				gInPauseMenu = true;
			}
			else if (touch.Pressed())
			{
				lastTP = touch.GetPos();
				touchPos = lastTP;

				if (inTechTree)
				{
					isDragScrolling = true;
					Technology tech = GetTechTreeBox(touchPos + techTreeScrollLoc);
					if (tech != Tech_Invalid)
					{
						if (techTreeDoubleTap > 0)
						{
							if (sim.CurrentResearch(localPlayer) == tech)
							{
								// cancel
								sim.StartResearch(localPlayer, Tech_Invalid);
								Play(Sound_Click);
							}
							else if (sim.CurrentResearch(localPlayer) == Tech_Invalid
									&& sim.GetHavePrerequisitesForTechnology(localPlayer, tech)
									&& !sim.GetHaveTechnology(localPlayer, tech))
							{
								sim.StartResearch(localPlayer, tech);
								DrawTechTree(subbg, sim, mainFont, localPlayer);
								Play(Sound_Click);
							}
							else if (sim.CurrentResearch(localPlayer) != Tech_Invalid)
							{
								Play(Sound_Error);
							}
						}
						else
						{
							techTreeDoubleTap = 30;
						}
					}
				}
				else
				{
					if (menuFade)
					{
						BuildingIndex bi = menu.Tap(touchPos);
						if (bi == BuildingIndex_Cancel)
						{
							menuFadeDelta = -1;
							count = 0;
							Play(Sound_Click);
							sim.StartBuilding(localPlayer, BuildingIndex_Invalid, V2());
						}
						else if (bi == BuildingIndex_WarBoat)
						{
							menuFadeDelta = -1;
							count = 0;
							Play(Sound_Click);
							sim.StartBuilding(localPlayer, BuildingIndex_WarBoat, V2());
							RefreshMenu(bg, sim, menu, localPlayer);
						}
						else if (bi != BuildingIndex_Invalid)
						{
							menuFadeDelta = -1;
							buildingToBuy = bi;
							count = 0;
							Play(Sound_Click);
						}
					}
					else if (tips.IsDisplaying())
					{
						tips.Dismiss(bg);
						Play(Sound_Click);
					}
					else
					{
						V2 touchSquare;
						map.GetMapLocation(scrollLoc, touchPos, touchSquare);
						if (gm == GameMode_PlaySavedInputs
								|| 
								!CanBuild(sim, map, buildingToBuy, touchSquare, localPlayerFlag, localPlayerReclaimFlag))
						{
							if (gm != GameMode_PlaySavedInputs
									&& !tips.HaveDisplayed(Tip_YourIsland)
									&& buildingToBuy != BuildingIndex_Invalid
									&& (map.Flags(touchSquare) & otherPlayerFlag))
							{
								tips.Add(Tip_YourIsland);
							}
							else
							{
								isDragScrolling = true;
								handCursor->Show();
								handCursor->pos = touchPos + scrollLoc;
							}
						}
						else if (buildingToBuy != BuildingIndex_Invalid)
						{
							if (CanBuild(sim, map, buildingToBuy, touchSquare, localPlayerFlag, localPlayerReclaimFlag))
							{
								if (buildingToBuy == BuildingIndex_Bulldozer)
								{
									if (sim.At(touchSquare) != BuildingIndex_Invalid && sim.At(touchSquare) != BuildingIndex_Destroyed)
									{
										sim.StartBuilding(localPlayer, buildingToBuy, touchSquare);
										buildingToBuy = BuildingIndex_Invalid;
									}
								}
								else
								{
									if (!tips.HaveDisplayed(Tip_OnlyOneBuilding) && sim.CurrentBuilding(localPlayer) != BuildingIndex_Invalid)
									{
										tips.Add(Tip_OnlyOneBuilding);
									}
									else
									{
										if (sim.At(touchSquare) != BuildingIndex_Invalid && sim.At(touchSquare) != BuildingIndex_ReclaimedLand && sim.At(touchSquare) != BuildingIndex_Destroyed)
										{
											Play(Sound_Error);
											tips.Add(Tip_BulldozerBeforeBuilding);
										}
										else if (sim.CurrentBuilding(localPlayer) == BuildingIndex_Invalid || sim.GetPercentageOfBuildingBuilt(localPlayer) < 80)
										{
											sim.StartBuilding(localPlayer, buildingToBuy, touchSquare);
											buildingToBuy = BuildingIndex_Invalid;
										}
									}
								}
							}
						}
					}
				}
			}
			else if (down & KEY_R)
			{
				if (menuFade)
				{
					Clear(bg, 0);
					menuFade = 0;
					menuFadeDelta = 0;
				}

				lcdSwap();
				inTechTree = !inTechTree;
				techTreeDoubleTap = 0;
				SwapToTechTree(inTechTree);
				if (inTechTree)
				{
					DrawTechTree(subbg, sim, mainFont, localPlayer);
					Play(Sound_MenuUp);
				}
				else
				{
					count = 0;
					Play(Sound_MenuDown);
				}
			}
			else if (touch.Released())
			{
				isDragScrolling = false;
				handCursor->Hide();
			}
			else if (touch.Held())
			{
				V2 tpv2(touch.GetPos());
				if (inTechTree)
				{
					if (isDragScrolling)
					{
						V2 delta = V2(lastTP) - tpv2;
						V2 deltaNormalized = delta.Normalize();
						if (delta.LengthSq() < 100 || deltaNormalized.Dot(lastDeltaNormalized) > F32(.6f))
						{
							techTreeScrollLoc += V2(lastTP) - tpv2;
							techTreeScrollLoc = techTreeScrollLoc.TruncateToInt();
						}
						lastDeltaNormalized = deltaNormalized;
					}
				}
				else
				{
					if (isDragScrolling)
					{
						V2 delta = V2(lastTP) - tpv2;
						V2 deltaNormalized = delta.Normalize();
						if (delta.LengthSq() < 100 || deltaNormalized.Dot(lastDeltaNormalized) > F32(.6f))
						{
							handCursor->pos = tpv2 + scrollLoc;
							scrollLoc += V2(lastTP) - tpv2;
							scrollLoc = scrollLoc.TruncateToInt();
						}
						lastDeltaNormalized = deltaNormalized;
					}
				}
				lastTP = tpv2;
			}

			if (menuFadeDelta)
			{
				BLEND_AB = menuFade | ((16 - menuFade) << 8);
				menuFade += menuFadeDelta;
				if (menuFade == 12)
				{
					menuFadeDelta = 0;
				}
				if (menuFade == 0)
				{
					menu.Erase(bg);
					menuFadeDelta = 0;
				}
			}
		}

		if (gm == GameMode_Scenario)
		{
			ScenarioResult sr = gScenario->Completed(sim);
			if (sr != ScenarioResult_NotComplete)
			{
				saver.SaveComplete();

				if (sr == ScenarioResult_CompletedGold)
				{
					messageBoxMessage = "Congratulations, you won a gold medal on this scenario!";
					Play(Sound_Win);
				}
				else if (sr == ScenarioResult_CompletedSilver)
				{
					messageBoxMessage = "Congratulations, you won a silver medal on this scenario. Try again and go for gold!";
					Play(Sound_Win);
				}
				else if (sr == ScenarioResult_CompletedBronze)
				{
					messageBoxMessage = "Congratulations, you won a bronze medal on this scenario. Try again and go for gold!";
					Play(Sound_Win);
				}
				else if (sr == ScenarioResult_Failed)
				{
					messageBoxMessage = "Scenario failed. Better luck next time.";
					Play(Sound_Lose);
				}
				break;
			}
		}

		if (gm == GameMode_PlaySavedInputs)
		{
			if (savedInputPlayer.Update())
			{
				delete gScenario;
				gScenario = 0;
				return;
			}
		}

		if (gm == GameMode_VsCpu)
		{
			if (sim.GetWinningPlayer() == localPlayer)
			{
				if (sim.GetWinType() == WinType_Population)
				{
					messageBoxMessage = "You have reached 100,000 population; you have won the expansionary battle.";
				}
				else if (sim.GetWinType() == WinType_Missiles)
				{
					messageBoxMessage = "Your nuclear missile barrage has destroyed the other island's civilization.";
				}
				Play(Sound_Win);
				break;
			}
			else if (sim.GetWinningPlayer() == otherPlayer)
			{
				if (sim.GetWinType() == WinType_Population)
				{
					messageBoxMessage = "The other player reached 100,000 population; you have lost the expansionary battle.";
				}
				else if (sim.GetWinType() == WinType_Missiles)
				{
					messageBoxMessage = "You have been destroyed by a nuclear missile barrage.";
				}
				Play(Sound_Lose);
				break;
			}
		}


		Technology curResearch = sim.CurrentResearch(localPlayer);
		if (curResearch != lastResearch)
		{
			if (lastResearch != Tech_Invalid && sim.GetHaveTechnology(localPlayer, lastResearch))
			{
				tips.Add(Tip_CompletedFirstResearch);
				Play(Sound_ResearchComplete, localPlayer);
				messageDisplay.Add(localPlayer, MessageType_ResearchComplete, lastResearch);
				if (menuFade && !tips.IsDisplaying() && !gInPauseMenu)
				{
					RefreshMenu(bg, sim, menu, localPlayer);
				}
			}
			if (inTechTree)
			{
				DrawTechTree(subbg, sim, mainFont, localPlayer);
			}
			lastResearch = curResearch;
		}

		for (int i = 0; i < 2; ++i)
		{
			BuildingIndex curBuilding = sim.CurrentBuilding(PlayerId(i));
			if (curBuilding != lastBuildings[i])
			{
				if (lastBuildings[i] == BuildingIndex_Invalid)
				{
					progressBuilds[i]->StartBuilding(&tiles_bin[(map.FirstBuildingIndex() + sim.CurrentBuilding(PlayerId(i))) * LG_ART_SQUARE_SIZE * LG_ART_SQUARE_SIZE], sim.BuildLocation(PlayerId(i)));
					progressBuilds[i]->Show();
					count = 0;
				}
				if (lastBuildings[i] != BuildingIndex_Invalid && sim.GetPercentageOfBuildingBuilt(PlayerId(i)) == F32(100))
				{
					Play(Sound_BuildingFinished, PlayerId(i));
				}
				if (menuFade && !tips.IsDisplaying() && !gInPauseMenu && i == localPlayer)
				{
					RefreshMenu(bg, sim, menu, localPlayer);
				}
				lastBuildings[i] = curBuilding;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			if (sim.CurrentBuilding(PlayerId(i)) != BuildingIndex_Invalid)
			{
				luvafair::F32 percent = sim.GetPercentageOfBuildingBuilt(PlayerId(i));
				progressBuilds[i]->SetPercentage(percent);
			}
			else
			{
				progressBuilds[i]->Hide();
			}
		}



		V2* l = &scrollLoc;
		if (inTechTree)
		{
			l = &techTreeScrollLoc;
		}
		if (held & KEY_LEFT) l->x -= 4;
		else if (held & KEY_RIGHT) l->x += 4;
		if (held & KEY_UP) l->y -= 4;
		else if (held & KEY_DOWN) l->y += 4;


		map.ValidateScroll(scrollLoc, edgeArrows);

		if (!gInPauseMenu) // todo; && not multiplayer
		{
			if (gm == GameMode_VsCpu)
			{
				cpu->Update();
			}
			sim.Update();
		}

		bool justBroughtUp = tips.Update(bg, down);
		if (justBroughtUp)
		{
			menuFade = 0;
		}
		if (tips.IsDisplaying() || gInPauseMenu)
		{
			BLEND_AB = 10 | ((16 - 10) << 8);
		}
		else
		{
			BLEND_AB = menuFade | ((16 - menuFade) << 8);
		}

		if (inTechTree)
		{
			if (techTreeScrollLoc.x.Int() < 0) techTreeScrollLoc.x = 0;
			if (techTreeScrollLoc.y.Int() < 0) techTreeScrollLoc.y = 0;
			if (techTreeScrollLoc.x.Int() >= LG_TECHTREE_BITMAP_WIDTH - SCREEN_WIDTH) techTreeScrollLoc.x = LG_TECHTREE_BITMAP_WIDTH - SCREEN_WIDTH;
			if (techTreeScrollLoc.y.Int() >= LG_TECHTREE_BITMAP_HEIGHT - SCREEN_HEIGHT) techTreeScrollLoc.y = LG_TECHTREE_BITMAP_HEIGHT - SCREEN_HEIGHT;

			SUB_BG3_CX = techTreeScrollLoc.x.Int() << 8;
			SUB_BG3_CY = techTreeScrollLoc.y.Int() << 8;

			techTreePaletteCycle += techTreePaletteCycleDelta;
			if (techTreePaletteCycle >= 40) techTreePaletteCycleDelta = -1;
			else if (techTreePaletteCycle <= 0) techTreePaletteCycleDelta = 1;
			BG_PALETTE_SUB[LG_TECHTREE_COLOUR_AVAILABLE_TO_RESEARCH] = RGB15(24, 11, 11);
			BG_PALETTE_SUB[LG_TECHTREE_COLOUR_RESEARCHING] = RGB15(11 - techTreePaletteCycle/5, 24 - techTreePaletteCycle/5, 11 - techTreePaletteCycle/5);

			if (techTreeDoubleTap) --techTreeDoubleTap;
		}
		else
		{
			if (count % 60 == 0)
			{
				Clear(statsOffscreenBuffer, 0x8000);

				messageDisplay.Update(statsOffscreenBuffer, mainFont, localPlayer);

				DrawStatus(statsOffscreenBuffer, mainFont, sim, buildingToBuy, gm);
			}
			++count;

			swiFastCopy(statsOffscreenBuffer, subbg, sizeof(statsOffscreenBuffer) / 4);
		}
		swiWaitForVBlank();

		objects.Update(scrollLoc);
		gSpriteHardware->Update();
		BG2_CX = scrollLoc.x.Int() << 8;
		BG2_CY = scrollLoc.y.Int() << 8;
	}

	if (messageBoxMessage)
	{
		Clear(bg, 0x8000);
		mainFont.DrawStringWrapped(bg, messageBoxMessage, 10, 10, SCREEN_WIDTH - 20, RGB15(31, 31, 31));
		BLEND_AB = 10 | ((16 - 10) << 8);
		for (;;)
		{
			touch.Update();
			if (touch.Released())
			{
				break;
			}
			swiWaitForVBlank();
		}
	}

	delete gScenario;
	gScenario = 0;
}

int main(void)
{
	sysSetBusOwners(true, true);
	MainHardwareInit();

	SpriteHardware::Init();
	Audio::Init();

	lg::TrackCheckpoint staticStuff;
	lg::TrackSaveCheckpoint(&staticStuff);

	for (;;)
	{
		GameMode gm = DoFrontEnd();
		assert(gm == GameMode_FreePlay || gm == GameMode_PlaySavedInputs || gm == GameMode_Scenario || gm == GameMode_VsCpu || gm == GameMode_LoadGame);

		lg::TrackDump(&staticStuff);

		DoBackEnd(gm);
		gSpriteHardware->Reset();

		lg::TrackDump(&staticStuff);
	}
}


