#ifndef LG_INCLUDED_audio_H
#define LG_INCLUDED_audio_H

class Audio
{
	static bool mMusic;
	static bool mSound;

	public:
		static void Init();

		static bool MusicIsEnabled();
		static bool SoundIsEnabled();

		static void ToggleMusicEnabled();
		static void ToggleSoundEnabled();
};

#endif
