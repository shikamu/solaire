#include <cassert>
#include <vector3d.h>
#include <irrlicht.h>

#include "Agent.h"
#include "HumanActuator.h"
#include "LogicScene.h"
#include "SpaceObjectFactory.h"
#include "RenderManager.h"
#include "HUDShader.h"
#include "System.h"
#include "RenderObjectIncludes.h"
#include "ConfigData.h"
#include "SpaceObject.h" 
#include "LogicConstants.h"
#include "System.h"
#include "TargettingModule.h"

using irr::scene::IMeshSceneNode;
using irr::core::vector3df;
using namespace irr; 

void Agent::clean()
{
	if(m_hudshader)
	{
		delete m_hudshader;
		m_hudshader = NULL;
	}
	if (m_targetshader)
	{
		delete m_targetshader;
		m_targetshader = NULL; 
	}
	if(m_TargetName)
	{
		m_TargetName->remove();
		m_TargetName = NULL;
	}
	if(m_reticule)
	{
		m_reticule->remove();
		m_reticule = NULL;
	}
	m_Score = 0;
	m_AgentID = 0;
	m_CurrentObject = NULL;
	m_Mask = 0;
	m_ParentScene = NULL;
	m_Shield = m_Armour = NULL;
}

void Agent::InitGUI()
{	
	LogicScene* parent = GetParentScene();
	assert(parent != NULL);
	irr::scene::ISceneManager* smgr = parent->getSceneManager();
	assert(smgr != NULL);

	RenderManager::get().GetCamera().Init(GetSpaceObject(), vector3df(0.0f, 150.0f, -300.0f), smgr);

	irr::scene::ICameraSceneNode* cam = RenderManager::get().GetCamera().GetNode();
	assert(cam != NULL);


	ShaderParams HUDparams;
	HUDparams.vertexShader = L"hudshader.hlsl";
	HUDparams.pixelShader = L"hudshader.hlsl";
	HUDparams.textures.push_back(TextureParam(0, "hudback.tga"));
	HUDparams.textures.push_back(TextureParam(1, "hudshield.tga"));
	HUDparams.textures.push_back(TextureParam(2, "hudarmour.tga"));
	m_hudshader = new HUDShader(HUDparams, m_Shield, m_Armour);

	IMeshSceneNode* hudNode = smgr->addMeshSceneNode(smgr->getMesh("Square.obj"), cam);
	if(hudNode)
	{
		hudNode->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		hudNode->setMaterialFlag(video::EMF_ZBUFFER, false);
		hudNode->setAutomaticCulling(scene::EAC_OFF);
		hudNode->setScale(vector3df(5000.0f, 5000.0f, 5000.0f));
		ShaderWrapper shader1(m_hudshader, hudNode, NULL);
	}	


	HUDparams.vertexShader = L"targetshader.hlsl";
	HUDparams.pixelShader = L"targetshader.hlsl";
	m_targetshader = new HUDShader(HUDparams, &m_TargetShield, &m_TargetArmour);
	
	m_TargetDataNode = smgr->addMeshSceneNode(smgr->getMesh("Square.obj"), cam);
	if(m_TargetDataNode)
	{
		m_TargetDataNode->setMaterialFlag(video::EMF_ZWRITE_ENABLE, false);
		m_TargetDataNode->setMaterialFlag(video::EMF_ZBUFFER, false);
		m_TargetDataNode->setAutomaticCulling(scene::EAC_OFF);
		m_TargetDataNode->setScale(vector3df(5000.0f, 5000.0f, 5000.0f));
		ShaderWrapper shader1(m_targetshader, m_TargetDataNode, NULL);
	}	
	m_TargetDataNode->setVisible(false); 
	
	//RenderManager::get().GetCamera().Init(GetSpaceObject(), vector3df(0.0f, 150.0f, -300.0f), smgr);

	m_reticule = smgr->addMeshSceneNode(smgr->getMesh("Square.obj"), cam);
	if(m_reticule)
	{
		m_reticule->setScale(core::vector3df(50.0f, 50.0f, 50.0f));
		m_reticule->setPosition(core::vector3df(-1.0f, 1.0f, 1000.0f));
		m_reticule->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Reticule.tga"));        
		m_reticule->setMaterialFlag(video::EMF_LIGHTING, false);
		m_reticule->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
	}		
	
	m_TargetName = System::get().getConfig()->getDevice()->getGUIEnvironment()->addStaticText(L"<No Target>", core::rect<s32>(120, 50, 320, 70), false);
	m_TargetName->setOverrideColor(video::SColor(255, 255, 255, 255));
	

	m_Arrow = GetParentScene()->getSceneManager()->addMeshSceneNode(GetParentScene()->getSceneManager()->getMesh("Arrow.obj"), m_CurrentObject->GetRenderObject()->GetSceneNode());
	if (m_Arrow)
	{
		m_Arrow->setScale(core::vector3df(50.0f, 50.0f, 50.0f));
		m_Arrow->setPosition(vector3df(0.0f, 0.0f, -15.0f));
		//m_Arrow->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("AutoCannonTexture.tga"));        
		//m_Arrow->setMaterialFlag(video::EMF_COLOR_MATERIAL, true);
		//m_Arrow->setMaterialFlag(video::EMF_LIGHTING, false);
		m_Arrow->setMaterialType(video::EMT_TRANSPARENT_ALPHA_CHANNEL);
		m_Arrow->getMaterial(0).EmissiveColor.set(255, 100, 100, 100);
		m_Arrow->getMaterial(0).ColorMask = irr::video::ECP_ALL;
	}

	
	scene::IParticleSystemSceneNode* dustNode = GetParentScene()->getSceneManager()->addParticleSystemSceneNode(true, RenderManager::get().GetCamera().GetNode(), 200, core::vector3df(0.0f, 0.0f, 00.0f), core::vector3df(00.0f, 0.0f, 0.0f), core::vector3df(1.0f, 1.0f,1.0f));
	if (dustNode)
	{
		dustNode->setMaterialTexture(0, System::get().getDevice()->getVideoDriver()->getTexture("Trail.tga"));
		dustNode->setMaterialType(video::EMT_TRANSPARENT_ADD_COLOR);
		dustNode->setMaterialFlag(video::EMF_LIGHTING, false);

		scene::IParticleEmitter* em = dustNode->createSphereEmitter(vector3df(0.0f, 0.0f, 100.0f), 100.0f, vector3df(0.0f, 0.0f, 0.0f),100, 100, 
			irr::video::SColor(255, 255, 255, 255), irr::video::SColor(255, 255, 255, 255), 10000, 10000, 360, irr::core::dimension2df(1.0f, 1.0f), irr::core::dimension2df(1.0f, 1.0f));

		dustNode->setEmitter(em);
		em->drop();
	}


}
void Agent::UpdateGUI(float dt)
{
	RenderManager::get().GetCamera().Update(dt);
	m_reticule->setVisible(GetSpaceObject()->GetActuator()->isFiring());

	updateTargetOutline(dt);
	if (GetSpaceObject()->GetHardTarget())
	{
		if (GetSpaceObject()->GetHardTarget()->GetSoftTarget())
		{
			if (GetSpaceObject()->GetHardTarget()->GetSoftTarget()->ID == GetSpaceObject()->ID)
			{
				m_TargetName->setDrawBorder(true);
			}
			else m_TargetName->setDrawBorder(false);
		}
		else m_TargetName->setOverrideColor(video::SColor(255, 0, 255, 50));

		m_TargetName->setText(GetSpaceObject()->GetHardTarget()->GetName().c_str());

		m_Arrow->setVisible(true);
		irr::core::matrix4 Inverse;
		GetSpaceObject()->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
		vector3df EnemyPos = GetSpaceObject()->GetHardTarget()->GetRenderObject()->GetPosition();
		Inverse.transformVect(EnemyPos);
		EnemyPos.normalize();
		m_Arrow->setRotation(EnemyPos.getHorizontalAngle());
		m_Arrow->getMaterial(0).EmissiveColor.set(255, 0, 255, 50);

		m_TargetShield = *GetSpaceObject()->GetHardTarget()->ShieldRemainingValue();
		m_TargetArmour = *GetSpaceObject()->GetHardTarget()->ArmourRemainingValue();
		m_TargetDataNode->setVisible(true);

	}
	else if (GetSpaceObject()->GetSoftTarget())
	{
		if (GetSpaceObject()->GetSoftTarget()->GetSoftTarget())
		{
			if (GetSpaceObject()->GetSoftTarget()->GetSoftTarget()->ID == GetSpaceObject()->ID)
			{
				m_TargetName->setDrawBorder(true);
			}
			else m_TargetName->setDrawBorder(false);
		}
		else m_TargetName->setOverrideColor(video::SColor(255, 0, 255, 50));
		m_TargetName->setText(GetSpaceObject()->GetSoftTarget()->GetName().c_str());

		m_Arrow->setVisible(true);		
		irr::core::matrix4 Inverse;
		GetSpaceObject()->GetRenderObject()->GetSceneNode()->getRelativeTransformation().getInverse(Inverse);
		vector3df EnemyPos = GetSpaceObject()->GetSoftTarget()->GetRenderObject()->GetPosition();
		Inverse.transformVect(EnemyPos);
		EnemyPos.normalize();
		m_Arrow->setRotation(EnemyPos.getHorizontalAngle());
		m_Arrow->getMaterial(0).EmissiveColor.set(255, 255, 100, 0);

		//m_TargetName->setOverrideColor(video::SColor(255, 255, 0, 0));
		TargettingModule* targettingModule = GetSpaceObject()->GetTargetter();
		if(targettingModule)
		{
			float fraction = targettingModule->getHardLockTimerFraction();
			irr::u32 r = irr::core::lerp(255, 0, std::min(1.0f, fraction));
			irr::u32 g = irr::core::lerp(0, 255, std::min(1.0f, fraction));
			m_TargetName->setOverrideColor(video::SColor(255, r, g, 0));
		}
		else
		{
			m_TargetName->setOverrideColor(video::SColor(255, 255, 0, 0));
		}

		m_TargetName->setText(GetSpaceObject()->GetSoftTarget()->GetName().c_str());
		m_Arrow->setVisible(true);

		m_TargetShield = *GetSpaceObject()->GetSoftTarget()->ShieldRemainingValue();
		m_TargetArmour = *GetSpaceObject()->GetSoftTarget()->ArmourRemainingValue();
		m_TargetDataNode->setVisible(true);
	}
	else
	{
		m_TargetName->setOverrideColor(video::SColor(255, 125, 125, 125));
		m_TargetName->setText(L"<No Target>");
		m_Arrow->setVisible(false);
		m_TargetDataNode->setVisible(false);
	}
}

void Agent::updateTargetOutline(const float dt)
{
	TargettingModule* module = NULL;
	SpaceObject* soft = NULL;
	if(m_CurrentObject && (soft = m_CurrentObject->GetSoftTarget()) && (module = m_CurrentObject->GetTargetter()))
	{
		SpaceObject* prev = m_ParentScene->GetSpaceObjectByID(module->getPreviousTarget());
		if(prev)
		{
			RenderObject* prevRobj = prev->GetRenderObject();
			irr::scene::IMeshSceneNode* prevOutlineNode = NULL;
			if(prevRobj && (prevOutlineNode = prevRobj->getOutlineNode()) && prevOutlineNode->isVisible())
			{
				irr::u32 alpha = static_cast<irr::u32>(irr::core::lerp((irr::u32)255, (irr::u32)0, module->getHardLockTimerFraction()));
				prevRobj->setOutlineShaderTime(alpha/255.0f);
				prevOutlineNode->setMaterialType((irr::video::E_MATERIAL_TYPE)prevRobj->getOutlineShaderMaterial());
				if(alpha < 5)
					prevOutlineNode->setVisible(false);
			}
		}

		RenderObject* robj = soft->GetRenderObject();
		irr::scene::IMeshSceneNode* outlineNode = NULL;
		if(robj && (outlineNode = robj->getOutlineNode()))
		{
			module->setHardLockTimerFraction(module->getHardLockTimer()/module->getHardTimer());
			outlineNode->setVisible(true);
			outlineNode->setMaterialType(irr::video::EMT_SOLID);
			if(m_CurrentObject->GetHardTarget())
			{
				module->setHardLockTimerFraction(1.0f);
				outlineNode->getMaterial(0).EmissiveColor.set(255, 0, 255, 0);
			}
			else
			{
				irr::u32 r = irr::core::lerp(255, 0, std::min(1.0f, module->getHardLockTimerFraction()));
				irr::u32 g = irr::core::lerp(0, 255, std::min(1.0f, module->getHardLockTimerFraction()));
				outlineNode->getMaterial(0).EmissiveColor.set(255, r, g, 0);
			}
		}
	}
}
