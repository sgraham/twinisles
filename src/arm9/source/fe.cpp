#include "lg/lg.h"
#include "fe.h"
#include "memory_config.h"
#include "sound_effects.h"
#include "lg/font.h"
#include "touch_smoother.h"
#include "mainlogo_bin.h"
#include "gui.h"
#include "audio.h"
#include "scenario.h"
#include "saved_inputs.h"

using namespace luvafair;

class FE
{
	Screen* mTop;
	Screen* mBottom;

public:

	FE() : mTop(0), mBottom(0) {}

	void SetTop(Screen* s) { mTop = s; mTop->Draw(); }
	void SetBottom(Screen* s) { mBottom = s; mBottom->Draw(); }
	void Tap(const V2& p)
	{
		mBottom->Tap(p);
	}
};

Screen* gScreenMain;
Screen* gScreenSingle;
Screen* gScreenScenarios;
Screen* gScreenMulti;
Screen* gScreenOptions;
Screen* gScreenCredits;
FE* gFE;
GameMode gExitFEWith;
bool gMusicEnabled = true;
bool gSoundEnabled = true;

void GoToMainMenu(GuiItem* item, Screen*, void *)
{
	gCurScenario = -1;
	gFE->SetBottom(gScreenMain);
}

void GoToSinglePlayerMenu(GuiItem* item, Screen*, void *)
{
	gFE->SetBottom(gScreenSingle);
}

void GoToScenarioMenu(GuiItem* item, Screen*, void *)
{
	gCurScenario = 0;
	gFE->SetBottom(gScreenScenarios);
}

void GoToOptionsMenu(GuiItem* item, Screen*, void *)
{
	gFE->SetBottom(gScreenOptions);
}

void GoToCreditsMenu(GuiItem* item, Screen*, void *)
{
	gFE->SetBottom(gScreenCredits);
}

void ToggleMusicText(GuiItem* item, Screen* scr, void *)
{
	Audio::ToggleMusicEnabled();
	if (Audio::MusicIsEnabled())
	{
		((Button*)item)->SetText("Music On");
	}
	else
	{
		((Button*)item)->SetText("Music Off");
	}
	scr->Draw();
}

void ToggleSoundText(GuiItem* item, Screen* scr, void *)
{
	Audio::ToggleSoundEnabled();
	if (Audio::SoundIsEnabled())
	{
		((Button*)item)->SetText("Sound On");
	}
	else
	{
		((Button*)item)->SetText("Sound Off");
	}
	scr->Draw();
}

void ExitWith(GuiItem* item, Screen* scr, void* p)
{
	gExitFEWith = (GameMode)(int)p;
}

static const char* sScenarios[] =
{
	"Grow your population to 500, before time expires. No CPU player.",
	"Grow your population to 1000, before time expires. No CPU player.",
	"Grow your population to 1200, before time expires. No CPU player.",
};
int gCurScenario;
WrappedLabel* gScenarioLabel;

void MoveThroughScenarios(GuiItem* item, Screen* scr, void* p)
{
	int d = (int)p;
	gCurScenario += d;
	if (gCurScenario < 0) gCurScenario = ARRAYSIZE(sScenarios) - 1;
	if (gCurScenario >= ARRAYSIZE(sScenarios)) gCurScenario = 0;
	gScenarioLabel->SetText(sScenarios[gCurScenario]);
	scr->Draw();
}

void CreateScenario()
{
	if (gCurScenario == -1)
	{
		gScenario = 0;
	}
	else if (gCurScenario == 0)
	{
		gScenario = new ScenarioTimedPopulation(0, 500, 60*60*7, 60*60/10*85, 60*60*10);
	}
	else if (gCurScenario == 1)
	{
		gScenario = new ScenarioTimedPopulation(0, 1000, 60*60*10, 60*60*11, 60*60*12);
	}
	else if (gCurScenario == 2)
	{
		gScenario = new ScenarioTimedPopulation(0, 1200, 60*60*14, 60*60/10*165, 60*60*18);
	}
	else
	{
		assert(false && "unhandled scenario");
	}
}

void StartScenario(GuiItem* item, Screen* scr, void* p)
{
	gExitFEWith = GameMode_Scenario;
}

GameMode DoFrontEnd()
{
	u16* subbg = (uint16*)BG_BMP_RAM_SUB(LG_OVERLAY_LAYER_BASE_SUB);
	u16* bg = (uint16*)BG_BMP_RAM(LG_OVERLAY_LAYER_BASE);

	TouchSmoother touch;

	BLEND_CR = BLEND_NONE;

	gExitFEWith = GameMode_Invalid;
	gCurScenario = -1;

	Screen top(subbg);
	top.Add(new Background(RGB8(40, 40, 80)));
#define LG_LOGO_WIDTH 242
#define LG_LOGO_HEIGHT 91
	top.Add(new Image((u16*)mainlogo_bin, SCREEN_WIDTH / 2 - LG_LOGO_WIDTH / 2, SCREEN_HEIGHT / 2 - LG_LOGO_HEIGHT / 2, LG_LOGO_WIDTH, LG_LOGO_HEIGHT));

	Screen main(bg);
	main.Add(new Background(RGB8(40, 40, 80)));
	main.Add(new Button("Single Player", 50, 20, SCREEN_WIDTH - 100, 35, GoToSinglePlayerMenu));
	main.Add(new Button("Multiplayer", 50, 60, SCREEN_WIDTH - 100, 35, 0));
	main.Add(new Button("Options", 50, 100, SCREEN_WIDTH - 100, 35, GoToOptionsMenu));
	main.Add(new Button("Credits", 50, 140, SCREEN_WIDTH - 100, 35, GoToCreditsMenu));
	main.Add(new Label("Release 3", 0, SCREEN_HEIGHT - 15, RGB8(56, 56, 96), TextAlignment_Left));
	gScreenMain = &main;

	Screen single(bg);
	single.Add(new Background(RGB8(40, 40, 80)));
	single.Add(new Button("Free Play", 50, 5, SCREEN_WIDTH - 100, 25, ExitWith, (void*)GameMode_FreePlay));
	single.Add(new Button("Scenarios", 50, 35, SCREEN_WIDTH - 100, 25, GoToScenarioMenu));
	single.Add(new Button("vs CPU", 50, 65, SCREEN_WIDTH - 100, 25, ExitWith, (void*)GameMode_VsCpu));

	Button* playReplay = new Button("Play Replay", 50, 95, SCREEN_WIDTH - 100, 25, ExitWith, (void*)GameMode_PlaySavedInputs);
	single.Add(playReplay);
	if (!SavedInputPlayer::HaveSavedReplay()) playReplay->SetCallback(0);

	Button* loadGame = new Button("Load Game", 50, 125, SCREEN_WIDTH - 100, 25, ExitWith, (void*)GameMode_LoadGame);
	single.Add(loadGame);
	if (!SavedInputPlayer::HaveSavedGame()) playReplay->SetCallback(0);

	single.Add(new Button("Back", 10, SCREEN_HEIGHT - 35, 70, 25, GoToMainMenu));
	gScreenSingle = &single;

	Screen credits(bg);
	credits.Add(new Background(RGB8(0, 0, 0)));
	credits.Add(new Label("Twin Isles by Scott Graham", SCREEN_WIDTH / 2, 10, RGB15(31, 31, 31), TextAlignment_Centre));
	credits.Add(new Label("Latest version: http://h4ck3r.net/", SCREEN_WIDTH / 2, 25, RGB15(15, 15, 15), TextAlignment_Centre));
	credits.Add(new Label("Feedback? twinisles@h4ck3r.net", SCREEN_WIDTH / 2, 40, RGB15(15, 15, 15), TextAlignment_Centre));
	credits.Add(new Label("Thanks to:", SCREEN_WIDTH / 2, 70, RGB15(8, 8, 8), TextAlignment_Centre));
	credits.Add(new Label("hona  .  Testing", SCREEN_WIDTH / 2, 85, RGB15(8, 8, 8), TextAlignment_Centre));
	credits.Add(new Label("maf/sdm&slc  .  Music", SCREEN_WIDTH / 2, 100, RGB15(8, 8, 8), TextAlignment_Centre));
	credits.Add(new Label("and everyone who worked on libnds", SCREEN_WIDTH / 2, 115, RGB15(8, 8, 8), TextAlignment_Centre));
	credits.Add(new Label("or figured out the hardware", SCREEN_WIDTH / 2, 130, RGB15(8, 8, 8), TextAlignment_Centre));
	credits.Add(new Button("Back", 10, SCREEN_HEIGHT - 35, 70, 25, GoToMainMenu));
	gScreenCredits = &credits;

	Screen options(bg);
	options.Add(new Background(RGB8(40, 40, 80)));
	options.Add(new Button(Audio::MusicIsEnabled() ? "Music On" : "Music Off", 50, 45, SCREEN_WIDTH - 100, 35, ToggleMusicText));
	options.Add(new Button(Audio::SoundIsEnabled() ? "Sound On" : "Sound Off", 50, 85, SCREEN_WIDTH - 100, 35, ToggleSoundText));
	options.Add(new Button("Back", 10, SCREEN_HEIGHT - 35, 70, 25, GoToMainMenu));
	gScreenOptions = &options;

	Screen scenarios(bg);
	scenarios.Add(new Background(RGB8(40, 40, 80)));
	scenarios.Add(new Arrow(10, 10, 16, 16, Arrow_Left, MoveThroughScenarios, (void*)-1));
	scenarios.Add(new Arrow(SCREEN_WIDTH - 10 - 16, 10, 16, 16, Arrow_Right, MoveThroughScenarios, (void*)1));
	gScenarioLabel = new WrappedLabel(sScenarios[0], 35, 10, SCREEN_WIDTH - 70, 105, RGB15(31, 31, 31));
	scenarios.Add(gScenarioLabel);
	scenarios.Add(new Button("Start", 70, 115, SCREEN_WIDTH - 140, 25, StartScenario));
	scenarios.Add(new Button("Back", 10, SCREEN_HEIGHT - 35, 70, 25, GoToMainMenu));
	gScreenScenarios = &scenarios;

	FE fe;
	gFE = &fe;
	fe.SetTop(&top);
	fe.SetBottom(&main);

	for (;;)
	{
		touch.Update();

		if (touch.Released())
		{
			fe.Tap(touch.GetPos());
		}

		if (gExitFEWith != GameMode_Invalid)
		{
			return gExitFEWith;
		}


		uint32 down = keysDown();
		uint32 up = keysUp();
		if (down & KEY_LID)
		{
			powerOFF(POWER_ALL);
		}
		else if (up & KEY_LID)
		{
			powerON(POWER_ALL);
		}

		swiWaitForVBlank();
	}
}
