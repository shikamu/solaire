#include <irrlicht.h>
#include "WarheadRenderObject.h"
#include "ShaderIncludes.h"
#include "System.h"
#include "LogicConstants.h"
#include "SpaceObject.h"

using namespace irr;

WarheadRenderObject::WarheadRenderObject()
{

}
WarheadRenderObject::~WarheadRenderObject()
{

}

const irr::io::path& WarheadRenderObject::GetPath(WARHEAD_TYPE type)
{
	switch (type)
	{
	case WARHEAD_ROCKET:
		return FILE_WARHEAD_ROCKET;
		break;
	case WARHEAD_EMP:
		return FILE_WARHEAD_EMP;
		break;
	case WARHEAD_MISSILE:
		return FILE_WARHEAD_MISSILE;
		break;
	case WARHEAD_TORPEDO:
		return FILE_WARHEAD_TORPEDO;
		break;
	default:
		return FILE_WARHEAD_ROCKET;
		break;
	}
}

void WarheadRenderObject::Init(irr::scene::ISceneManager* smgr, SpaceObject* object, SpaceObject* host, WARHEAD_TYPE type)
{
	ShaderParams params1;
	params1.vertexShader = L"WarheadShader.hlsl";
	params1.pixelShader = L"WarheadShader.hlsl";
	WarheadShader* m_warheadshader = new WarheadShader(params1, object->ObjectMask, smgr, static_cast<irr::scene::ILightSceneNode*>(smgr->getSceneNodeFromId(LIGHT_ID)));
	StoreShader(m_warheadshader);

	irr::scene::IMeshSceneNode* m_ObjectNode = smgr->addMeshSceneNode(smgr->getMesh(GetPath(type)));

	if(m_ObjectNode)
	{
		m_ObjectNode->setPosition(host->GetRenderObject()->GetPosition());
		m_ObjectNode->setRotation(host->GetRenderObject()->GetRotation());
		ShaderWrapper shader1(m_warheadshader, m_ObjectNode, NULL);		
	}

	
	scene::IParticleSystemSceneNode* jetNode = smgr->addParticleSystemSceneNode(true, m_ObjectNode, 200, core::vector3df(0.0f, 0.0f, -20.0f), m_ObjectNode->getRotation(), core::vector3df(0.3f, 0.3f, 1.0f));
	if (jetNode)
	{
		
		scene::IParticleEmitter* em = jetNode->createCylinderEmitter(vector3df(0.0f, 0.0f, 0.0f), 10.0f, vector3df(0.f, 0.f, -1.0f), 10.0f, false, vector3df(0.0f, 0.0f, 0.0f), 240, 240,
			irr::video::SColor(128, 200, 200, 200), irr::video::SColor(128, 200, 200, 200), 500, 500, 360, irr::core::dimension2df(15.0f, 15.0f), irr::core::dimension2df(15.0f, 15.0f));								
		scene::IParticleAffector *Fader = jetNode->createFadeOutParticleAffector(video::SColor(0,0,0,0), 500);
		

		jetNode->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Trail.tga"));
		jetNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		jetNode->setMaterialFlag(video::EMF_LIGHTING, false);
		jetNode->setEmitter(em);
		jetNode->addAffector(Fader);

		em->drop();
		Fader->drop();
	}	
	SetSceneNode(m_ObjectNode);
}

void WarheadRenderObject::Init(irr::scene::ISceneManager* smgr, SpaceObject* object, vector3df& pos, vector3df& rot, WARHEAD_TYPE type)
{
	ShaderParams params1;
	params1.vertexShader = L"ProjectileShader.hlsl";
	params1.pixelShader = L"ProjectileShader.hlsl";
	ProjectileShader* m_bulletshader = new ProjectileShader(params1, smgr, (PROJECTILE_TYPE)type);
	StoreShader(m_bulletshader);
	
	irr::scene::IMeshSceneNode* m_ObjectNode = smgr->addMeshSceneNode(smgr->getMesh(GetPath(type)));

	if(m_ObjectNode)
	{
		m_ObjectNode->setPosition(pos);
		m_ObjectNode->setRotation(rot);
		ShaderWrapper shader1(m_bulletshader, m_ObjectNode, NULL);		
	}

	scene::IParticleSystemSceneNode* jetNode = smgr->addParticleSystemSceneNode(true, m_ObjectNode, 200, core::vector3df(0.0f, 0.0f, -20.0f), m_ObjectNode->getRotation(), core::vector3df(0.3f, 0.3f, 1.0f));
	if (jetNode)
	{
		
		scene::IParticleEmitter* em = jetNode->createCylinderEmitter(vector3df(0.0f, 0.0f, 0.0f), 10.0f, vector3df(0.f, 0.f, -1.0f), 10.0f, false, vector3df(0.0f, 0.0f, 0.0f), 240, 240,
			irr::video::SColor(128, 200, 200, 200), irr::video::SColor(128, 200, 200, 200), 500, 500, 360, irr::core::dimension2df(15.0f, 15.0f), irr::core::dimension2df(15.0f, 15.0f));								
		scene::IParticleAffector *Fader = jetNode->createFadeOutParticleAffector(video::SColor(0,0,0,0), 500);
		

		jetNode->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Trail.tga"));
		jetNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		jetNode->setMaterialFlag(video::EMF_LIGHTING, false);
		jetNode->setEmitter(em);
		jetNode->addAffector(Fader);

		em->drop();
		Fader->drop();
	}	

	SetSceneNode(m_ObjectNode);
}