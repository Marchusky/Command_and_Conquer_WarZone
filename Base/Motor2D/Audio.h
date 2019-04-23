#ifndef __Audio_H__
#define __Audio_H__

#include "Module.h"
#include "Timer.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;
struct Mix_Chunk;

enum FX
{
	MIN1 = 1,
	MIN3,
	MIN5,
	MIN10,
	BARRACKS_BUILD,
	BUILDING_EXPLOSION,
	BUILDING_REPAIR,
	BUTTON_SLIDE,
	CHANGE_FOCUS,
	EXIT,
	FINAL_EXPLOSION,
	INGAME_CLICK,
	INVINCIBLE,
	LAUNCH_MISSILES,
	MINE_BUILD,
	PLAY,
	SENTRYGUN_ATTACK,
	SENTRYGUN_BUILD,
	SLIDER_DOWN,
	SLIDER_UP,
	SOLDIER_ATTACK,
	TESLA_BUILD,
	TESLA_REPAIR,
	TESLA_ATTACK,
	TIMERSTART,
	TIMERSTOP,
	WRONG
};

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called every frame 
	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fade_time = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	unsigned int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(unsigned int fx, int repeat = 0);

	void AdjustMusicVol(int value);
	void AdjustSoundVol(int value);
	void PauseMusic();
	void FadeOut(const char* path);
	void FadeIn();

	bool song1played = false;
	bool song2played = false;
	bool fading_in = false;
	bool fading_out = false;
	bool reading_time = true;

	int fade_time = 0;
	int volume_before = 0; 

	uint SetMusicVolume() const
	{
		return Mix_VolumeMusic(masterVolume * (musicVolume * MIX_MAX_VOLUME / 100) / 100);
	}
	uint SetSfxVolume() const {
		return Mix_Volume(-1, masterVolume * (sfxVolume * MIX_MAX_VOLUME / 100) / 100);
	}

public:

	unsigned short masterVolume = 100;
	unsigned short musicVolume = 30;
	unsigned short sfxVolume = 30;

public:
	string			folder_music;
	string			folder_fx;

	vector<string>	tracks_path;
	vector<string>	fx_path;


private:

	_Mix_Music*			music = nullptr;
	list<Mix_Chunk*>	fx;

	string track;
};

#endif // __Audio_H__