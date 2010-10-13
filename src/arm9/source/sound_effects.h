#ifndef LG_INCLUDED_sound_effects_H
#define LG_INCLUDED_sound_effects_H

#include "playerid.h"

enum SoundEffect
{
	Sound_Click,
	Sound_Balloon,
	Sound_MenuUp,
	Sound_MenuDown,
	Sound_PlaceBuilding,
	Sound_BuildingFinished,
	Sound_Destroy,
	Sound_Error,
	Sound_ResearchComplete,
	Sound_Win,
	Sound_Lose,
	Sound_War,

	Sound_Count
};

extern PlayerId SoundEffectLocalPlayer;
void Play(SoundEffect effect);
void Play(SoundEffect effect, PlayerId player);

#endif
