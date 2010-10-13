#include "lg/lg.h"
#include "audio.h"
#include "Sound9.h"
#include "basketmn_mod.h"

bool Audio::mMusic;
bool Audio::mSound;

void ModCallback(u32 rowParam, BOOL rowTick)
{
	if (rowParam == 0)
	{
		SndStopMOD();
		SndPlayMOD(basketmn_mod);
		SndSetMODCallback(ModCallback);
	}
}

void Audio::Init()
{
	SndInit9();

	mMusic = false;
	mSound = true;
	ToggleMusicEnabled();
}

bool Audio::MusicIsEnabled() { return mMusic; }
bool Audio::SoundIsEnabled() { return mSound; }

void Audio::ToggleMusicEnabled()
{
	mMusic = !mMusic;
	if (mMusic)
	{
		SndPlayMOD(basketmn_mod);
		SndSetMODCallback(ModCallback);
	}
	else
	{
		SndStopMOD();
	}
}

void Audio::ToggleSoundEnabled()
{
	mSound = !mSound;
}
