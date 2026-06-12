#include <irrlicht.h>
#include "AutoCamera.h"
#include "SpaceObject.h"
#include "RenderObjectIncludes.h"
#include "Actuator.h"

using namespace irr::scene; 
using namespace irr::video;

AutoCamera::AutoCamera() : m_Target (NULL), m_Smgr (NULL), m_Cam (NULL), dX (0.0f), dY (0.0f), dZ (0.0f)
{
}
AutoCamera::~AutoCamera()
{
}

void AutoCamera::Init(SpaceObject* target, const vector3df& offset, irr::scene::ISceneManager* smgr)
{
	SetTarget(target);
	SetOffset(offset);
	SetSceneManager(smgr);

	m_Cam = m_Smgr->addCameraSceneNode(m_Target->GetRenderObject()->GetSceneNode(), m_Target->GetRenderObject()->GetSceneNode()->getPosition() - m_Offset, vector3df(0, 0, 0));
	m_Cam->setFarValue(10000.0f);
	m_Cam->setPosition(offset);

	//AxisY = smgr->addMeshSceneNode(
 //               smgr->addArrowMesh( "FighterTexture.tga",
 //                               SColor(255, 255, 0, 0),
 //                               SColor(255, 0, 255, 0),
 //                               16,16,
 //                               200.f, 100.3f,
 //                               10.1f, 10.6f
 //                               ));

}

void AutoCamera::Update(const float dt)
{	
	vector3df RotationOffset(0.0f, 0.0f, 0.0f);
	Actuator* act = m_Target->GetActuator();

	//AxisY->setPosition(m_Target->GetRenderObject()->GetPosition());
	
	if (act)
	{
		vector3df CamPos = m_Offset;
		
		//m_InPitch += (InputValue - m_InPitch) * dt * 15.0f;
		dX += (act->GetControlValues().X - dX) * dt * 10.0f;  
		dY += (act->GetControlValues().Y - dY) * dt * 10.0f;  
		dZ += (act->GetControlValues().Z - dZ) * dt * 10.0f;  


		CamPos.rotateXYBy(dZ * 20.0f, vector3df(0.0f, 0.0f, 0.0f));
		CamPos.rotateXZBy(dY * 20.0f, vector3df(0.0f, 0.0f, 0.0f));
		CamPos.rotateYZBy(-dX * 20.0f, vector3df(0.0f, 0.0f, 0.0f));

		m_Cam->setPosition(CamPos * act->getZoom());
	}

	m_Cam->setTarget(m_Target->GetRenderObject()->GetPosition() + m_Target->GetRenderObject()->GetRotation().rotationToDirection(vector3df(0.0f, 0.0f, 1.0f)* 200.0f));
	vector3df UpVector = m_Target->GetRenderObject()->GetRotation().rotationToDirection(vector3df(0.0f, 1.0f, 0.0f));
	//UpVector.rotateXYBy(dZ * 20.0f, vector3df(0.0f, 0.0f, 0.0f));
	m_Cam->setUpVector(UpVector);
}

void AutoCamera::SetSceneManager(irr::scene::ISceneManager* smgr)
{
	m_Smgr = smgr;
}
void AutoCamera::SetTarget(SpaceObject* target)
{
	m_Target = target;
}
void AutoCamera::SetOffset(const vector3df& offset)
{
	m_Offset = offset;
}
irr::scene::ICameraSceneNode* AutoCamera::GetNode()
{
	return m_Cam; 
}

