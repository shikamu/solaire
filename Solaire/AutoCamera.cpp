#include <irrlicht.h>
#include "AutoCamera.h"
#include "SpaceObject.h"
#include "RenderObjectIncludes.h"
#include "Actuator.h"

using namespace irr::scene; 
using namespace irr::video;

AutoCamera::AutoCamera() : m_Target (NULL), m_Smgr (NULL), m_Cam (NULL), dX (0.0f), dY (0.0f), dZ (0.0f), m_FixedFollow (false)
{
}
AutoCamera::~AutoCamera()
{
}

void AutoCamera::Init(SpaceObject* target, const vector3df& offset, irr::scene::ISceneManager* smgr)
{
	//This object lives in the RenderManager singleton, so it survives across scenes. The old
	//camera node was already destroyed with the previous scene's manager, leaving m_Cam dangling
	//- clear it (and the spectator flag) BEFORE SetTarget, which would otherwise dereference the
	//stale camera. The fresh camera is created below.
	m_Cam = NULL;
	m_FixedFollow = false;

	SetSceneManager(smgr);
	SetOffset(offset);
	SetTarget(target);

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
	if(!m_Target || !m_Cam || !m_Target->GetRenderObject())
		return;

	const vector3df shipPos = m_Target->GetRenderObject()->GetPosition();
	const vector3df shipRot = m_Target->GetRenderObject()->GetRotation();

	if(m_FixedFollow)
	{
		//Spectating: the camera is parented to the root node (see SetTarget), so position it in
		//WORLD space behind/above the watched ship. This way the camera is never destroyed if
		//the ship it's watching gets removed from the scene.
		irr::core::matrix4 rot;
		rot.setRotationDegrees(shipRot);
		vector3df worldOffset = m_Offset;
		rot.transformVect(worldOffset);
		m_Cam->setPosition(shipPos + worldOffset);
		m_Cam->setTarget(shipPos + shipRot.rotationToDirection(vector3df(0.0f, 0.0f, 1.0f) * 200.0f));
		m_Cam->setUpVector(shipRot.rotationToDirection(vector3df(0.0f, 1.0f, 0.0f)));
		return;
	}

	//Normal player-follow: the camera is parented to our own ship, so the offset is in its
	//local frame.
	Actuator* act = m_Target->GetActuator();
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
	else
	{
		m_Cam->setPosition(m_Offset);
	}

	m_Cam->setTarget(shipPos + shipRot.rotationToDirection(vector3df(0.0f, 0.0f, 1.0f) * 200.0f));
	m_Cam->setUpVector(shipRot.rotationToDirection(vector3df(0.0f, 1.0f, 0.0f)));
}

void AutoCamera::SetSceneManager(irr::scene::ISceneManager* smgr)
{
	m_Smgr = smgr;
}
void AutoCamera::SetTarget(SpaceObject* target)
{
	m_Target = target;
	if(!m_Cam)
		return;//Init creates the camera parented to the first target right after this call

	if(m_FixedFollow)
	{
		//Spectating: parent the camera (and the HUD nodes hanging off it) to the always-present
		//root node. Parenting it to the watched ship would destroy the camera with the ship when
		//that ship is removed from the scene. Update() then chases the ship in world space.
		if(m_Smgr)
			m_Cam->setParent(m_Smgr->getRootSceneNode());
	}
	else if(target && target->GetRenderObject() && target->GetRenderObject()->GetSceneNode())
	{
		//Normal follow: parent to our own (stable) ship so the local-space offset works.
		m_Cam->setParent(target->GetRenderObject()->GetSceneNode());
	}
}
void AutoCamera::SetOffset(const vector3df& offset)
{
	m_Offset = offset;
}
irr::scene::ICameraSceneNode* AutoCamera::GetNode()
{
	return m_Cam; 
}

