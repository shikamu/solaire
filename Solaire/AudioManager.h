#pragma once

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <map>
#include <vector>
#include <queue>

#include "../fmod/inc/fmod.h"
#include "../fmod/inc/fmod.hpp"
#include <vector3d.h>
#include "Task.h"
#include "CSLock.h"

using irr::core::vector3df;

namespace FMODAudio
{
	struct Listener
	{
		int id; 
		vector3df position, velocity, direction, up;
	};

	enum AUDIO_TYPE
	{
		AUDIO_FX,
		AUDIO_MUSIC
	};

	class AudioClip;
	class Playlist; 

	class AudioManager : public Task
	{
		friend class Playlist; 
		private:
			static AudioManager m_audio_manager;

			AudioManager();
			~AudioManager();

			FMOD_RESULT result;
			FMOD_SPEAKERMODE speakermode;
			FMOD_CAPS caps;
			FMOD::System *system;
			FMOD::Channel *ClipChannel;
			FMOD::Channel *MusicChannel;
			FMOD::Sound *sound;
			FMOD::Sound *music;
			unsigned int version;
			int numdrivers;
			bool _init, _deinit;

			float ClipVolume, MusicVolume, MusicVolumeLimit;
			bool MusicPaused, m_FadeOut, m_FadeIn, SurroundEnabled;
			std::string CurrentMusic;
			//void play_fx(AudioClip *inClip);
			FMOD_VECTOR convert_vector(vector3df &in);
			void setup();
			void update(const float dt);
			int init(); 
			void deinit();
			
			CSLock m_SoundmapLock;
			std::map<unsigned int, AudioClip*> m_Soundmap; 

			CSLock m_PlaylistLock;
			std::vector<Playlist*> m_Playlists;  
			void UpdatePlaylist(const float dt);
			Playlist* m_CurrentPlaylist; 
			Playlist* m_NextPlaylist;

			void FadeOut();
			void FadeIn();
			AudioClip* PlayClip(unsigned int id);
	public: 
			// Returns a handle to the audio manager.
			static AudioManager &get();
			unsigned int __stdcall run();
			
			void update_listener(Listener &in);
			void number_of_listeners(int i);
			void toggle_3D(bool in);

			void set_volume(float value, AUDIO_TYPE Type);
			void PlayFX(unsigned int id);

			unsigned int GetClipID(const char *filename);

			unsigned int CreateClip(const char *filename, AUDIO_TYPE type);
			unsigned int CreatePlaylist(const char* name); 

			void AddToPlaylist(unsigned int id);
			void AddToPlaylist(const char* listName, unsigned int id);

			void PlayMusic();
			void PauseMusic();
			void StopMusic();
			void SelectPlaylist(const char* name); 
			void NextTrack();
			void PrevTrack();
	};
};