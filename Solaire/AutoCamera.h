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
public:
	AutoCamera();
	~AutoCamera();
	void Init(SpaceObject* target, const vector3df& offset, irr::scene::ISceneManager* smgr); 
	void Update(const float dt); 

	void SetSceneManager(irr::scene::ISceneManager* smgr);
	void SetTarget(SpaceObject* target);
	void SetOffset(const vector3df& offset);
	irr::scene::ICameraSceneNode* GetNode();
};