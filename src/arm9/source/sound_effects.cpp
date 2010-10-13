#include "lg/lg.h"
#include "sound_effects.h"
#include "Sound9.h"
#include "audio.h"

#include "click_bin.h"
#include "balloon_bin.h"
#include "menuup_bin.h"
#include "menudown_bin.h"
#include "place_building_bin.h"
#include "building_finished_bin.h"
#include "destroy_bin.h"
#include "error_bin.h"
#include "research_complete_bin.h"
#include "win_bin.h"
#include "lose_bin.h"
#include "war_bin.h"

PlayerId SoundEffectLocalPlayer(-1);

void Play(SoundEffect effect)
{
	if (!Audio::SoundIsEnabled()) return;

	const u8* data = 0;
	switch (effect)
	{
		case Sound_Click: data = click_bin; break;
		case Sound_Balloon: data = balloon_bin; break;
		case Sound_MenuUp: data = menuup_bin; break;
		case Sound_MenuDown: data = menudown_bin; break;
		case Sound_PlaceBuilding: data = place_building_bin; break;
		case Sound_BuildingFinished: data = building_finished_bin; break;
		case Sound_Destroy: data = destroy_bin; break;
		case Sound_Error: data = error_bin; break;
		case Sound_ResearchComplete: data = research_complete_bin; break;
		case Sound_Win: data = win_bin; break;
		case Sound_Lose: data = lose_bin; break;
		case Sound_War: data = war_bin; break;
		default: assert(false && "sound effect not handled"); return;
	}

	SndPlaySample(data, 127);
}

void Play(SoundEffect effect, PlayerId id)
{
	if (!Audio::SoundIsEnabled()) return;

	assert(SoundEffectLocalPlayer != -1);
	if (SoundEffectLocalPlayer == id)
	{
		Play(effect);
	}
}
