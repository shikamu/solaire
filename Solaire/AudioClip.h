#pragma once
#include "AudioManager.h"

namespace FMODAudio
{
	class FMODObject
	{
		friend class AudioManager;
		private:
			int m_id;
			FMOD_VECTOR m_position, m_velocity, m_forward, m_up;
			
		public:
			// RELOCATE TO PROTECTED
			FMODObject();
			FMODObject(int id);
				
			void Set(vector3df &inPos, vector3df &inVel, vector3df &inForward, vector3df &inUp);
			void Set(Listener &in);
				
			int id();
			FMOD_VECTOR & position();
			FMOD_VECTOR & velocity();
			FMOD_VECTOR & direction();
			FMOD_VECTOR & up();
	};

	class AudioClip
	{
		public:	
			AudioClip();
			AudioClip(AUDIO_TYPE inType);
			~AudioClip();
			int id;
			std::string filename;
			AUDIO_TYPE type;
			FMOD::Sound* data;
			unsigned int length; 

			vector3df position, velocity;
	};

};