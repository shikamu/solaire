#include "Playlist.h"
#include "AudioClip.h"

using namespace FMODAudio;

Playlist::Playlist(AudioManager* host) : m_ID(0), m_Name (""), m_Host(host), m_CurrentTrack (NULL), m_TrackProgress (0.0f), m_IsActive (false)
{
}
Playlist::~Playlist()
{
}

void Playlist::Name(const char* name)
{
	m_Name = name; 
}
const char* Playlist::Name()
{
	return m_Name.c_str();
}
void Playlist::ID(unsigned int id)
{
	m_ID = id;
}
unsigned int Playlist::ID()
{
	return m_ID; 
}

void Playlist::Clear()
{
	while (!m_Playlist.empty()) m_Playlist.pop();
	m_CurrentTrack = NULL;
}

void Playlist::AddTrack(unsigned int id)
{
	m_Playlist.push(id);
}

void Playlist::Update(const float dt)
{
	if (m_Playlist.size() == 0) return;
	if (!m_CurrentTrack)
	{
		unsigned int nextID = m_Playlist.front();
		m_Playlist.pop();
		m_Playlist.push(nextID);
		m_TrackProgress = 0.0f;
		
		m_CurrentTrack = m_Host->PlayClip(nextID);
		m_Host->FadeIn();
	}
	m_TrackProgress += dt; 

	if (m_TrackProgress > m_CurrentTrack->length * 0.001f)
	{
		// Next track

		unsigned int nextID = m_Playlist.front();
		m_Playlist.pop();
		m_Playlist.push(nextID);
		m_TrackProgress = 0.0f;
		m_Host->StopMusic();
		m_CurrentTrack = m_Host->PlayClip(nextID);
		m_Host->FadeIn();
	}
	else if (m_TrackProgress > m_CurrentTrack->length * 0.001f - 2.1f)
	{
		m_Host->FadeOut();
	}
}
void Playlist::NextTrack()
{
	m_TrackProgress = m_CurrentTrack->length * 0.001f - 2.0f; 
}

void Playlist::PrevTrack()
{
	int ListSize = m_Playlist.size(); 
	for (int i = 0; i < ListSize - 1; i++)
	{
		m_Playlist.push(m_Playlist.front());
		m_Playlist.pop(); 
	}

	m_TrackProgress = m_CurrentTrack->length * 0.001f - 2.0f; 
}

void Playlist::Stop()
{
	m_Host->StopMusic();
	m_CurrentTrack = NULL; 
}