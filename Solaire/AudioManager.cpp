#include "audioManager.h"
#include "Timer.h"
#include "AudioClip.h"
#include "Playlist.h"
using namespace FMODAudio;



/* AudioManager */
AudioManager AudioManager::m_audio_manager;

AudioManager::AudioManager() 
	: ClipVolume (0.5f), MusicVolume (0.5f), MusicVolumeLimit(0.5f),
	  MusicPaused (false), CurrentMusic (""), m_FadeOut (false),
	  m_FadeIn (false), SurroundEnabled (false), m_CurrentPlaylist(NULL), m_NextPlaylist(NULL)
{}

AudioManager::~AudioManager()
{
	finish();
}

AudioManager &AudioManager::get()
{
	return m_audio_manager;
}

int AudioManager::init()
{
	if (!_init)
	{
		char name[256];
		result = FMOD::System_Create(&system);
		result = system->getVersion(&version);
		if (version < FMOD_VERSION)
		{
			printf("Error! You are using an old version of FMOD %08x. This program requires %08x\n", version, FMOD_VERSION);
			return 0;
		}
		else
		{
			result = system->getNumDrivers(&numdrivers);
			if (numdrivers == 0)
			{
				result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);

			}
			else
			{
				result = system->getDriverCaps(0, &caps, 0, &speakermode);

				/*
				Set the user selected speaker mode.
				*/
				result = system->setSpeakerMode(speakermode);
				if (caps & FMOD_CAPS_HARDWARE_EMULATED)
				{
					/*
					The user has the 'Acceleration' slider set to off! This is really bad
					for latency! You might want to warn the user about this.
					*/
					result = system->setDSPBufferSize(1024, 10);
				}
				result = system->getDriverInfo(0, name, 256, 0);
				if (strstr(name, "SigmaTel"))
				{
					/*
					Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
					PCM floating point output seems to solve it.
					*/
					result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
				}
			}
			result = system->init(100, FMOD_INIT_NORMAL, 0);
			if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
			{
				/*
				Ok, the speaker mode selected isn't supported by this soundcard. Switch it
				back to stereo...
				*/
				result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
				/*
				... and re-init.
				*/
				result = system->init(100, FMOD_INIT_NORMAL, 0);
			}
			//result = system->setSoftwareChannels(5);

			_init = true; 
			return 1;
		}
	}
	else return 0; 
}

void AudioManager::update(const float dt) 
{ 
	FMOD_RESULT res = system->update(); 

	m_PlaylistLock.Lock();
	if (m_NextPlaylist && MusicVolume < 0.02f)
	{
		m_CurrentPlaylist->Stop();
		m_CurrentPlaylist = m_NextPlaylist;
		m_NextPlaylist = NULL; 
	}
	if (m_CurrentPlaylist)
	{
		m_CurrentPlaylist->Update(dt);
	}
	m_PlaylistLock.Unlock();
	if (m_FadeOut)
	{
		if (MusicVolume > 0.0f)
		{
			MusicVolume -= MusicVolumeLimit * dt * 0.5f;
			MusicChannel->setVolume(MusicVolume);
		}
		else
		{
			MusicVolume = 0.0f;
			MusicChannel->setVolume(MusicVolume);
			m_FadeOut = false; 
		}
	}
	else if (m_FadeIn)
	{
		if (MusicVolume < MusicVolumeLimit)
		{
			MusicVolume += MusicVolumeLimit * dt * 0.5f;
			MusicChannel->setVolume(MusicVolume);
		}
		else
		{
			MusicVolume = MusicVolumeLimit;
			MusicChannel->setVolume(MusicVolume);
			m_FadeIn = false; 
		}
	}
}

void AudioManager::deinit()
{
	if (!_deinit)
	{
		//std::map<int, FMOD::Sound*>::iterator _it;
		//for (_it = _soundmap.begin(); _it != _soundmap.end(); _it++) 
		//	(*_it).second->release();
		m_SoundmapLock.Lock();
		for (auto i = m_Soundmap.begin(); i != m_Soundmap.end(); i++)
		{
			(*i).second->data->release();
			delete (*i).second; 
		}
		m_PlaylistLock.Lock();
		for (auto i = m_Playlists.begin(); i < m_Playlists.end(); i++)
		{
			delete (*i);
		}
		m_Playlists.clear();
		m_PlaylistLock.Unlock();
		m_Soundmap.clear();
		m_SoundmapLock.Unlock();
		system->release();
		_deinit = true; 
	}
}

//void AudioManager::play_fx(AudioClip *inClip)
//{
//	if (inClip != NULL)
//	{
//	if (inClip->id == -1)
//	{
//		inClip->id = (int) _soundmap.size(); 
//		FMOD::Sound *buff = NULL;
//		_soundmap.insert(std::pair<int, FMOD::Sound*>(inClip->id, buff));
//
//
//		if (SurroundEnabled)
//			result = system->createSound(inClip->filename.c_str(), FMOD_3D | FMOD_LOOP_OFF, 0, &_soundmap.rbegin()->second);
//		else
//			result = system->createSound(inClip->filename.c_str(), FMOD_DEFAULT | FMOD_LOOP_OFF, 0, &_soundmap.rbegin()->second);
//		//Sound Pointer Initialisation
//		/////////////////////////////////
//		//_soundmap.rbegin()->second->setMode(FMOD_LOOP_OFF);
//
//	}
//				
//	result = system->playSound(FMOD_CHANNEL_FREE, _soundmap.find(inClip->id)->second, false, &ClipChannel);
//	ClipChannel->setVolume(ClipVolume);
//	if (SurroundEnabled)
//	{
//		FMODObject Temp;
//		Temp.Set(inClip->position, inClip->velocity, inClip->position, inClip->velocity);
//		ClipChannel->set3DAttributes(&Temp.position(), &Temp.velocity());
//	}
//	}
//}


FMOD_VECTOR AudioManager::convert_vector(vector3df &in)
{
	FMOD_VECTOR Temp;
	Temp.x = (float)in.X;
	Temp.y = (float)in.Y;
	Temp.z = (float)in.Z;
	return Temp;
}

//void AudioManager::play_fx_file(const char *filename)
//{
//	if (_init)
//	{
//		if (sound == NULL)
//		{
//			result = system->createSound(filename, FMOD_DEFAULT, 0, &sound);
//			result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &ClipChannel);
//			ClipChannel->setVolume(ClipVolume);
//		}
//		else
//		{
//			result = system->playSound(FMOD_CHANNEL_FREE, sound, false, &ClipChannel);
//		}
//	}
//}
//
//void AudioManager::play_music_file(const char *filename)
//{
//	if (_init)
//	{
//		if ((music == NULL) || (std::string(filename).compare(CurrentMusic) != 0))
//		{
//			result = system->createStream(filename, FMOD_DEFAULT, 0, &music);
//			result = system->playSound(FMOD_CHANNEL_FREE, music, false, &MusicChannel);
//			MusicChannel->setVolume(MusicVolume);
//			CurrentMusic = filename;
//		}
//		else
//		{
//			result = system->playSound(FMOD_CHANNEL_FREE, music, false, &MusicChannel);
//		}
//	}
//}
//
//void AudioManager::play_audio(AudioClip *inClip)
//{
//	if (_init && inClip)
//	{
//		if (inClip->type == 0) play_fx(inClip);
//		else play_music(inClip);
//	}
//}
//


void AudioManager::set_volume(float value, AUDIO_TYPE Type)
{
	if (Type == AUDIO_FX)
	{
		ClipVolume = value;
	}
	else if (Type == AUDIO_MUSIC)
	{
		MusicVolumeLimit = value; 
		MusicVolume = value; 
		MusicChannel->setVolume(MusicVolume);
	}
}

void AudioManager::FadeOut()
{
	m_FadeOut = true;
}

void AudioManager::FadeIn()
{
	m_FadeIn = true;
}

void AudioManager::update_listener(Listener &in)
{
	FMODObject Temp;
	Temp.Set(in.position, in.velocity, in.direction, in.up);
	system->set3DListenerAttributes(Temp.id(), &Temp.position(), &Temp.velocity(), &Temp.direction(), &Temp.up());
}

void AudioManager::number_of_listeners(int i)
{
	result = system->set3DNumListeners(i);
}

void AudioManager::toggle_3D(bool in)
{
	SurroundEnabled = in;
}


AudioClip* AudioManager::PlayClip(unsigned int id)
{
	m_SoundmapLock.Lock();
	std::map<unsigned int, AudioClip*>::iterator it = m_Soundmap.find(id);
	if (it == m_Soundmap.end()) 
	{
		m_SoundmapLock.Unlock();
		return NULL; 
	}
	if (!it->second)
	{
		m_SoundmapLock.Unlock();
		return NULL; 
	}
	if (!it->second->data)
	{
		m_SoundmapLock.Unlock();
		return NULL; 
	}

	switch (it->second->type)
	{
	case AUDIO_FX:
		{
			result = system->playSound(FMOD_CHANNEL_FREE, it->second->data, false, &ClipChannel);
		}
		break;
	case AUDIO_MUSIC:
		{
			result = system->playSound(FMOD_CHANNEL_FREE, it->second->data, false, &MusicChannel);
		}
		break; 
	}
	m_SoundmapLock.Unlock();
	return it->second;
}

void AudioManager::PlayFX(unsigned int id)
{
	PlayClip(id);
}

void AudioManager::UpdatePlaylist(const float dt)
{
	m_PlaylistLock.Lock();
		if (m_CurrentPlaylist) m_CurrentPlaylist->Update(dt);
	m_PlaylistLock.Unlock();
}

unsigned int AudioManager::GetClipID(const char *filename)
{
	unsigned int Output = 0;
	m_SoundmapLock.Lock();
	for (auto i = m_Soundmap.begin(); i != m_Soundmap.end(); i++)
	{
		if ((*i).second->filename.compare(filename) == 0)
			Output = (*i).first;
	}
	m_SoundmapLock.Unlock();
	return Output;
}

unsigned int AudioManager::CreateClip(const char *filename, AUDIO_TYPE type)
{
	unsigned int ExistingID = GetClipID(filename);
	if (ExistingID != 0) return ExistingID;

	AudioClip* clip = new AudioClip(type);
	clip->id = m_Soundmap.size() + 1;
	clip->filename = filename; 
	result = system->createSound(filename, FMOD_DEFAULT | FMOD_LOOP_OFF, 0, &clip->data);
	clip->data->getLength(&clip->length,1);
	m_SoundmapLock.Lock();
	m_Soundmap.insert(std::pair<int, AudioClip*>(clip->id, clip));
	m_SoundmapLock.Unlock();
	return clip->id; 
}

unsigned int AudioManager::CreatePlaylist(const char* name)
{
	m_PlaylistLock.Lock();
	for (auto i = m_Playlists.begin(); i < m_Playlists.end(); i++)
	{
		
		if (strcmp((*i)->Name(),name) == 0)
		{
			return 0;
		}
	}
	Playlist* list = new Playlist(this);
	list->Name(name);
	list->ID(m_Playlists.size() + 1);
	m_Playlists.push_back(list);
	m_PlaylistLock.Unlock();
	return list->ID();
}

void AudioManager::AddToPlaylist(unsigned int id)
{
	m_PlaylistLock.Lock();
	if (m_CurrentPlaylist)
		m_CurrentPlaylist->AddTrack(id);
	m_PlaylistLock.Unlock();
}
void AudioManager::AddToPlaylist(const char* listName, unsigned int id)
{
	m_PlaylistLock.Lock();
	for (auto i = m_Playlists.begin(); i < m_Playlists.end(); i++)
	{
		if (strcmp((*i)->Name(),listName) == 0)
			(*i)->AddTrack(id);
	}

	m_PlaylistLock.Unlock();
}

void AudioManager::PlayMusic()
{
	m_PlaylistLock.Lock();
	m_PlaylistLock.Unlock();

}
void AudioManager::PauseMusic()
{	
	MusicPaused = !MusicPaused; 
	result = MusicChannel->setPaused(MusicPaused);
}
void AudioManager::StopMusic()
{
	MusicChannel->stop();
}

void AudioManager::SelectPlaylist(const char* name)
{
	m_PlaylistLock.Lock();

	Playlist* SelectedList = NULL;
	for (auto i = m_Playlists.begin(); i < m_Playlists.end(); i++)
	{
		if (strcmp((*i)->Name(),name) == 0)
		{
			SelectedList = (*i);
		}		
	}

	if (SelectedList)
	{
		if (m_CurrentPlaylist)
		{
			m_NextPlaylist = SelectedList;
			FadeOut();
		}
		else m_CurrentPlaylist = SelectedList; 
	}

	m_PlaylistLock.Unlock();
}

//
//void AudioManager::stop_music()
//{	
//	MusicPaused = true; 
//	result = MusicChannel->setPaused(MusicPaused);
//}
//
//void AudioManager::start_music()
//{	
//	MusicPaused = false; 
//	result = MusicChannel->setPaused(MusicPaused);
//}

void AudioManager::NextTrack()
{
	if (m_CurrentPlaylist) m_CurrentPlaylist->NextTrack();
}
void AudioManager::PrevTrack()
{
	if (m_CurrentPlaylist) m_CurrentPlaylist->PrevTrack();
}

unsigned int __stdcall AudioManager::run()
{
	init();
	toggle_3D(false);
	Timer audioTimer;
	audioTimer.start();
	float currentTime = (float)audioTimer.getTimeSec();

	CreatePlaylist("Menu");
	CreatePlaylist("Game");

	AddToPlaylist("Menu", CreateClip("data/MenuMusic.mp3", AUDIO_MUSIC));
	AddToPlaylist("Menu", CreateClip("data/MenuMusic.mp3", AUDIO_MUSIC));
	AddToPlaylist("Menu", CreateClip("data/TestMusic1.mp3", AUDIO_MUSIC));

	AddToPlaylist("Game", CreateClip("data/TestMusic1.mp3", AUDIO_MUSIC));
	AddToPlaylist("Game", CreateClip("data/TestMusic1.mp3", AUDIO_MUSIC));
	AddToPlaylist("Game", CreateClip("data/MenuMusic.mp3", AUDIO_MUSIC));

	CreateClip("data/Karl.mp3", AUDIO_FX);

	SelectPlaylist("Menu");

	while (!isInterrupted())
	{
		float newTime = (float)audioTimer.getTimeSec();
		if (newTime - currentTime < 0.01f) continue; 
		update(newTime - currentTime);
		currentTime = newTime;
	}
	deinit();
	return 1; 
}