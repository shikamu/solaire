#pragma once
#include <queue>
#include <string>
#include "AudioManager.h"

namespace FMODAudio
{
	class AudioClip;

	class Playlist
	{
	private:
		std::string m_Name;
		unsigned int m_ID;
		AudioClip* m_CurrentTrack; 
		AudioManager* m_Host; 
		float m_TrackProgress;
		std::queue<unsigned int> m_Playlist;
		bool m_IsActive; 
	public:

		Playlist(AudioManager* host);
		~Playlist();
		void AddTrack(unsigned int id);
		void NextTrack();
		void PrevTrack();
		void Update(const float dt); 
		void Clear(); 

		void Name(const char* name);
		const char* Name();
		void ID(unsigned int id);
		unsigned int ID();
		void Stop();
	};
};