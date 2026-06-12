#include "AudioClip.h"

using namespace FMODAudio;

AudioClip::AudioClip(AUDIO_TYPE inType) 
	: id(-1), type(inType), data(NULL), length(0), filename("")
{}

AudioClip::AudioClip()
	: id(-1), type(AUDIO_FX), data(NULL), length(0), filename("")
{}

AudioClip::~AudioClip()
{
}

FMODObject::FMODObject() 
	: m_id (0)
{}

FMODObject::FMODObject(int id)
	: m_id (id) 
{}

void FMODObject::Set(vector3df &inPos, vector3df &inVel, vector3df &inForward, vector3df &inUp)
{
	m_position.x = inPos.X;
	m_position.y = inPos.Y;
	m_position.z = inPos.Z;
	m_velocity.x = inVel.X;
	m_velocity.y = inVel.Y;
	m_velocity.z = inVel.Z;
	m_forward.x = inForward.X;
	m_forward.y = inForward.Y;
	m_forward.z = inForward.Z;
	m_up.x = inUp.X; 
	m_up.y = inUp.Y; 
	m_up.z = inUp.Z; 
}

void FMODObject::Set(Listener &in)
{
	m_position.x = (float)in.position.X;
	m_position.y = (float)in.position.Y;
	m_position.z = (float)in.position.Z;
	m_velocity.x = (float)in.velocity.X;
	m_velocity.y = (float)in.velocity.Y;
	m_velocity.z = (float)in.velocity.Z;
	m_forward.x = (float)in.direction.X;
	m_forward.y = (float)in.direction.Y;
	m_forward.z = (float)in.direction.Z;
	m_up.x = (float)in.up.X; 
	m_up.y = (float)in.up.Y; 
	m_up.z = (float)in.up.Z; 
}

int FMODObject::id()
{
	return m_id;
}

FMOD_VECTOR & FMODObject::position()
{
	return m_position;
}

FMOD_VECTOR & FMODObject::velocity()
{
	return m_velocity;
}

FMOD_VECTOR & FMODObject::direction()
{
	return m_forward;
}

FMOD_VECTOR & FMODObject::up()
{
	return m_up;
}