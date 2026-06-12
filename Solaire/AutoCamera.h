#pragma once
#include <vector3d.h>

using irr::core::vector3df;

class SpaceObject;

namespace irr
{
	namespace scene
	{
		class ICameraSceneNode;
		class ISceneManager; 
	}
}

class AutoCamera
{
private:
	SpaceObject* m_Target;
	irr::scene::ISceneManager* m_Smgr; 
	irr::scene::ICameraSceneNode* m_Cam;
	//irr::scene::ISceneNode* AxisX; 
	irr::scene::ISceneNode* AxisY; 
	//irr::scene::ISceneNode* AxisZ; 
	vector3df m_Offset;
	float dX, dY, dZ;
	bool m_FixedFollow;
public:
	AutoCamera();
	~AutoCamera();
	void Init(SpaceObject* target, const vector3df& offset, irr::scene::ISceneManager* smgr); 
	void Update(const float dt); 

	void SetSceneManager(irr::scene::ISceneManager* smgr);
	void SetTarget(SpaceObject* target);
	void SetOffset(const vector3df& offset);
	//When true, use a steady chase offset instead of the player-input-driven one. Used for
	//spectating another ship (whose controls we don't drive).
	void SetFixedFollow(const bool b) { m_FixedFollow = b; }
	irr::scene::ICameraSceneNode* GetNode();
};