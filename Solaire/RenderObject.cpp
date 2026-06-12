#include <irrlicht.h>
#include "RenderObject.h"
#include "ShaderIncludes.h"
#include "System.h"
#include "LogicConstants.h"
#include "SpaceObject.h"

#include "OutlineFadeoutShader.h"

using namespace irr;
//using irr::scene::IParticleSystemSceneNode;


RenderObject::~RenderObject() 
{
	for (auto i = m_ShaderList.begin(); i != m_ShaderList.end(); i++)
	{
		delete (*i); 
	}
} 

void RenderObject::Update(const float dt)
{
}
void RenderObject::StoreShader(Shader* in)
{
	m_ShaderList.push_back(in);
}

void RenderObject::SetSceneNode(irr::scene::IMeshSceneNode* node)
{
	m_RootNode = node;
}
irr::scene::IMeshSceneNode* RenderObject::GetSceneNode()
{
	return m_RootNode;
}

void RenderObject::SetRotation(const vector3df &rot)
{
	GetSceneNode()->setRotation(rot);
}
const vector3df& RenderObject::GetRotation()
{
	return GetSceneNode()->getRotation();
}
void RenderObject::SetPosition(const vector3df &pos)
{
	GetSceneNode()->setPosition(pos);
}
const vector3df& RenderObject::GetPosition()
{
	return GetSceneNode()->getPosition();
}

irr::scene::IMeshSceneNode* RenderObject::getOutlineNode()
{
	return NULL;
}

void ShipRenderObject::Init(irr::scene::ISceneManager* smgr, SpaceObject* object)
{


	ShaderParams params1, params2;
	params1.vertexShader = L"ShipShader.hlsl";
	params1.pixelShader = L"ShipShader.hlsl";
	params1.textures.push_back(TextureParam(0, "FighterTexture.tga"));
	params1.textures.push_back(TextureParam(1, "FighterAlpha.tga"));
	ShipShader* m_shipshader = new ShipShader(params1, object->ObjectMask, smgr, static_cast<irr::scene::ILightSceneNode*>(smgr->getSceneNodeFromId(LIGHT_ID)), object->ShieldRemainingValue(), object->ShieldImpactTime(), object->ShieldImpactVector());
	StoreShader(m_shipshader);
	
	irr::scene::IMesh* hull = NULL;
	if ((object->ObjectMask & MASK_GROUP_1) != 0)
	{
		hull = smgr->getMesh("OrionHi.obj");
	}
	else if ((object->ObjectMask & MASK_GROUP_2) != 0)
	{
		hull = smgr->getMesh("OrionHi.obj");
	}
	else if ((object->ObjectMask & MASK_GROUP_3) != 0)
	{
		hull = smgr->getMesh("ScorpiusHi.obj");
	}
	else  hull = smgr->getMesh("ScorpiusHi.obj");


	irr::scene::IMeshSceneNode* m_shipNode = smgr->addMeshSceneNode(hull);
	//irr::scene::IMeshSceneNode* m_shipNode = smgr->addSphereSceneNode(150.0f);
	if(m_shipNode)
	{
		m_shipNode->setPosition(core::vector3df(0, 0, 0));    
		ShaderWrapper shader1(m_shipshader, m_shipNode, NULL);
	}

	m_outlineNode = smgr->addMeshSceneNode(hull, m_shipNode, SHIP_OUTLINE_ID);
	if(m_outlineNode)
	{
		m_outlineNode->setPosition(core::vector3df(0, 0, 0));
		m_outlineNode->setScale(core::vector3df(1.05f, 1.05f, 1.05f));
		
		ShaderParams params3;
		params3.vertexShader = L"OutlineFadeoutShader.hlsl";
		params3.pixelShader = L"OutlineFadeoutShader.hlsl";
		//float* time = new float;
		//time = 0.5f;
		m_outlineShaderTime = 1.0f;
		
		OutlineFadeoutShader* outlineShader = new OutlineFadeoutShader(params3, &m_outlineShaderTime);
		StoreShader(outlineShader);
		//lol = outlineShader->getMaterial();
		m_outlineShaderMaterial = outlineShader->getMaterial();

		//m_outlineNode->getMaterial(0).ColorMaterial = irr::video::ECM_EMISSIVE;
		//m_outlineNode->getMaterial(0).ColorMaterial = irr::video::ECM_NONE;
        m_outlineNode->getMaterial(0).EmissiveColor.set(255,255,0,0);
		//m_outlineNode->getMaterial(0).DiffuseColor.set(255,255,0,0);
        m_outlineNode->setMaterialFlag(video::EMF_BACK_FACE_CULLING, false);
        m_outlineNode->setMaterialFlag(video::EMF_FRONT_FACE_CULLING, true);
		m_outlineNode->getMaterial(0).ColorMask = irr::video::ECP_ALL;
		//m_outlineNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		m_outlineNode->setMaterialType(video::EMT_SOLID);
		//m_outlineNode->setMaterialFlag(video::EMF_LIGHTING, false);
		//m_outlineNode->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		//m_outlineNode->setMaterialType(video::EMT_TRANSPARENT_VERTEX_ALPHA);
		

		m_outlineNode->setVisible(false);
	}
	
	irr::scene::IParticleSystemSceneNode* m_EngineNodeLeft = smgr->addParticleSystemSceneNode(true, m_shipNode, 200, core::vector3df(-91.0f, -00.0f, -120.0f), m_shipNode->getRotation(), core::vector3df(1.0f, 1.0f, 1.0f));
	irr::scene::IParticleSystemSceneNode* m_EngineNodeRight = smgr->addParticleSystemSceneNode(true, m_shipNode, 200, core::vector3df(91.0f, -00.0f, -120.0f), m_shipNode->getRotation(), core::vector3df(1.0f, 1.0f, 1.0f));

	if (m_EngineNodeLeft && m_EngineNodeRight)
	{
		scene::IParticleEmitter* em = m_EngineNodeLeft->createSphereEmitter(vector3df(0.0f, 0.0f, 0.0f), 2.0f, vector3df(0.0f, 0.0f, 0.0f),240, 240, 
			irr::video::SColor(128, 200, 200, 200), irr::video::SColor(128, 200, 200, 200), 500, 1000, 360, 
			irr::core::dimension2df(15.0f, 15.0f), irr::core::dimension2df(15.0f, 15.0f));

		m_EngineNodeLeft->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Exhaust.tga"));
		m_EngineNodeLeft->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		m_EngineNodeLeft->setMaterialFlag(video::EMF_LIGHTING, false);
		m_EngineNodeLeft->setEmitter(em);

		m_EngineNodeRight->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Exhaust.tga"));
		m_EngineNodeRight->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		m_EngineNodeRight->setMaterialFlag(video::EMF_LIGHTING, false);
		m_EngineNodeRight->setEmitter(em);

		m_EngineNodeLeft->setParticlesAreGlobal(false);
		m_EngineNodeRight->setParticlesAreGlobal(false);
		em->drop();
	}





	scene::IParticleSystemSceneNode* jetNodeLeft = smgr->addParticleSystemSceneNode(true, m_shipNode, 200, core::vector3df(-91.0f, -00.0f, -120.0f), m_shipNode->getRotation(), core::vector3df(1.0f, 1.0f, 1.0f));
	scene::IParticleSystemSceneNode* jetNodeRight = smgr->addParticleSystemSceneNode(true, m_shipNode, 200, core::vector3df(91.0f, -00.0f, -120.0f), m_shipNode->getRotation(), core::vector3df(1.0f, 1.0f, 1.0f));
	if (jetNodeLeft && jetNodeRight)
	{
		
		//scene::IParticleEmitter* em = jetNode->createPointEmitter(vector3df(0.0f, 0.0f, 0.0f), 60, 60, irr::video::SColor(128, 255, 255, 255), irr::video::SColor(128, 255, 255, 255),
		//	1000, 1000, 0, irr::core::dimension2df(25.0f, 25.0f), irr::core::dimension2df(35.0f, 35.0f));
		//scene::IParticleEmitter* em = jetNodeLeft->createSphereEmitter(vector3df(0.0f, 0.0f, 0.0f), 5.0f, vector3df(0.0f, 0.0f, 0.0f),240, 240, 
		//	irr::video::SColor(128, 200, 200, 200), irr::video::SColor(128, 200, 200, 200), 1000, 1000, 360, irr::core::dimension2df(15.0f, 15.0f), irr::core::dimension2df(15.0f, 15.0f));
		scene::IParticleEmitter* em = jetNodeLeft->createCylinderEmitter(vector3df(0.0f, 0.0f, 0.0f), 10.0f, vector3df(0.f, 0.f, -1.0f), 10.0f, false, vector3df(0.0f, 0.0f, 0.0f), 240, 240,
			irr::video::SColor(128, 200, 200, 200), irr::video::SColor(128, 200, 200, 200), 1000, 1000, 360, irr::core::dimension2df(15.0f, 15.0f), irr::core::dimension2df(15.0f, 15.0f));
		//									
		scene::IParticleAffector *Fader = jetNodeLeft->createFadeOutParticleAffector(video::SColor(0,0,0,0), 500);
		

		jetNodeLeft->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Trail.tga"));
		jetNodeLeft->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		jetNodeLeft->setMaterialFlag(video::EMF_LIGHTING, false);
		jetNodeLeft->setEmitter(em);
		jetNodeLeft->addAffector(Fader);


		jetNodeRight->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Trail.tga"));
		jetNodeRight->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		jetNodeRight->setMaterialFlag(video::EMF_LIGHTING, false);
		jetNodeRight->setEmitter(em);
		jetNodeRight->addAffector(Fader);

		em->drop();
		Fader->drop();
	}	

	SetSceneNode(m_shipNode);
}


void ShipRenderObject::Update(const float dt)
{
}

irr::scene::IMeshSceneNode* ShipRenderObject::getOutlineNode()
{
	return m_outlineNode;
}
