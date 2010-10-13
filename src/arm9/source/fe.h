#ifndef LG_INCLUDED_fe_H
#define LG_INCLUDED_fe_H

enum GameMode
{
	GameMode_Invalid = -1,
	GameMode_FreePlay,
	GameMode_Scenario,
	GameMode_PlaySavedInputs,
	GameMode_VsCpu,
	GameMode_LoadGame,
};

GameMode DoFrontEnd();

extern int gCurScenario;
void CreateScenario();

#endif
