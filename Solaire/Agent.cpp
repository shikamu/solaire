#include <cassert>
#include <cmath>
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
#include "InputConstants.h"
#include "KeyBindings.h"

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
	//Must run before the camera update so the camera never follows a freed ship.
	handleSpectatorInput();

	RenderManager::get().GetCamera().Update(dt);

	//Estimate the current target's velocity from how far it moved this frame, for the lead
	//indicator. The client never receives target velocity, so we derive it from position; the
	//smoothing damps per-frame noise (and the interpolated client position already eases it).
	{
		SpaceObject* me = GetSpaceObject();
		SpaceObject* tgt = me ? (me->GetHardTarget() ? me->GetHardTarget() : me->GetSoftTarget()) : NULL;
		if(tgt && tgt->GetRenderObject() && dt > 0.0001f)
		{
			vector3df tp = tgt->GetRenderObject()->GetPosition();
			if(tgt->ID == m_LastTargetID)
			{
				vector3df newVel = (tp - m_LastTargetPos) / dt;
				m_EstimatedTargetVel += (newVel - m_EstimatedTargetVel) * std::min(1.0f, dt * 8.0f);
			}
			else
			{
				m_EstimatedTargetVel = vector3df(0.0f, 0.0f, 0.0f);//target just changed
			}
			m_LastTargetPos = tp;
			m_LastTargetID = tgt->ID;
		}
		else
		{
			m_EstimatedTargetVel = vector3df(0.0f, 0.0f, 0.0f);
			m_LastTargetID = 0;
		}
	}

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

void Agent::handleSpectatorInput()
{
	if(!m_ParentScene)
		return;

	KeyBindings& kb = System::get().getConfig()->getKeybindings();
	const bool toggle = kb.isTriggered(SPECTATE_TOGGLE);
	const bool next = kb.isTriggered(SPECTATE_NEXT);
	const bool prev = kb.isTriggered(SPECTATE_PREV);

	if(toggle && !m_prevSpecToggle)
	{
		if(m_Spectating)
			stopSpectating();
		else
			cycleSpectate(1);//enter, watching the first other ship
	}

	if(m_Spectating)
	{
		//If the ship we were watching has gone, hop to another (or exit if none remain).
		if(!m_ParentScene->GetSpaceObjectByID(m_SpectateID))
		{
			cycleSpectate(1);
		}
		else
		{
			if(next && !m_prevSpecNext) cycleSpectate(1);
			if(prev && !m_prevSpecPrev) cycleSpectate(-1);
		}
	}

	m_prevSpecToggle = toggle;
	m_prevSpecNext = next;
	m_prevSpecPrev = prev;
}

void Agent::cycleSpectate(int direction)
{
	//Gather every other ship currently in the scene.
	std::vector<SpaceObject*> ships;
	SpaceObject* own = GetSpaceObject();
	const std::map<unsigned int, SpaceObject*>& objs = m_ParentScene->GetObjectList();
	for(std::map<unsigned int, SpaceObject*>::const_iterator it = objs.begin(); it != objs.end(); ++it)
	{
		SpaceObject* s = it->second;
		if(s && s != own && (s->ObjectMask & MASK_SHIP) != 0 && s->GetRenderObject())
			ships.push_back(s);
	}

	if(ships.empty())
	{
		stopSpectating();//nothing to watch
		return;
	}

	int idx = 0;
	if(m_Spectating)
	{
		for(size_t i = 0; i < ships.size(); ++i)
		{
			if(ships[i]->ID == m_SpectateID) { idx = (int)i; break; }
		}
		idx += direction;
		idx = ((idx % (int)ships.size()) + (int)ships.size()) % (int)ships.size();//wrap around
	}

	SpaceObject* target = ships[idx];
	m_Spectating = true;
	m_SpectateID = target->ID;
	RenderManager::get().GetCamera().SetFixedFollow(true);
	RenderManager::get().GetCamera().SetTarget(target);
}

void Agent::stopSpectating()
{
	m_Spectating = false;
	m_SpectateID = 0;
	RenderManager::get().GetCamera().SetFixedFollow(false);
	if(GetSpaceObject())
		RenderManager::get().GetCamera().SetTarget(GetSpaceObject());
}

void Agent::DrawHUD()
{
	SpaceObject* me = GetSpaceObject();
	if(!me || !me->GetRenderObject() || !me->GetRenderObject()->GetSceneNode() || !m_ParentScene)
		return;

	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	scene::ISceneManager* smgr = m_ParentScene->getSceneManager();
	scene::ICameraSceneNode* cam = RenderManager::get().GetCamera().GetNode();
	if(!driver || !smgr || !cam)
		return;

	const core::dimension2du screen = driver->getScreenSize();
	const vector3df myPos = me->GetRenderObject()->GetPosition();
	const unsigned int myGroup = me->ObjectMask & GROUP_FILTER;

	// ---------- Spectator banner ----------
	if(m_Spectating)
	{
		SpaceObject* spec = m_ParentScene->GetSpaceObjectByID(m_SpectateID);
		stringw label(L"SPECTATING");
		if(spec)
		{
			label += L": ";
			label += spec->GetName();
		}
		//Show the player's actual (rebindable) keys, the same friendly strings the Options menu uses.
		KeyBindings& kb = System::get().getConfig()->getKeybindings();
		label += L"      [";
		label += kb.getBindingFor(SPECTATE_TOGGLE, true);
		label += L"] back to your ship      [";
		label += kb.getBindingFor(SPECTATE_PREV, true);
		label += L" / ";
		label += kb.getBindingFor(SPECTATE_NEXT, true);
		label += L"] cycle ships";
		gui::IGUIFont* font = System::get().getDevice()->getGUIEnvironment()->getBuiltInFont();
		if(font)
		{
			const core::dimension2du td = font->getDimension(label.c_str());
			const s32 tx = ((s32)screen.Width - (s32)td.Width) / 2;
			font->draw(label.c_str(), core::recti(tx, 30, tx + (s32)td.Width, 30 + (s32)td.Height), video::SColor(255, 255, 255, 0));
		}
	}

	// ---------- Radar: top-down blips, rotating with the ship ----------
	const float radarRange = 8000.0f;//world units that map to the radar's edge
	const s32 radarRadius = 80;
	const core::position2di radarCenter((s32)(screen.Width / 2), (s32)screen.Height - radarRadius - 20);

	driver->draw2DPolygon(radarCenter, (f32)radarRadius, video::SColor(160, 0, 220, 0), 32);
	driver->draw2DRectangle(video::SColor(255, 220, 220, 220), core::recti(radarCenter.X - 2, radarCenter.Y - 2, radarCenter.X + 2, radarCenter.Y + 2));//me, at the centre

	//Rotation-only inverse (ignore the node's scale/translation) so radar distances stay in
	//true world units; we subtract my position ourselves to get the relative vector.
	core::matrix4 inv;
	inv.setRotationDegrees(me->GetRenderObject()->GetRotation());
	inv.makeInverse();

	const std::map<unsigned int, SpaceObject*>& objs = m_ParentScene->GetObjectList();
	for(std::map<unsigned int, SpaceObject*>::const_iterator it = objs.begin(); it != objs.end(); ++it)
	{
		SpaceObject* obj = it->second;
		if(!obj || obj == me) continue;
		if((obj->ObjectMask & MASK_SHIP) == 0) continue;//ships only
		if(!obj->GetRenderObject()) continue;

		vector3df rel = obj->GetRenderObject()->GetPosition() - myPos;
		inv.transformVect(rel);//into my local frame: X = right, Z = forward
		if(rel.getLength() > radarRange) continue;

		s32 bx = radarCenter.X + (s32)((rel.X / radarRange) * radarRadius);
		s32 by = radarCenter.Y - (s32)((rel.Z / radarRange) * radarRadius);//forward -> up on the radar

		const bool friendly = ((obj->ObjectMask & GROUP_FILTER) == myGroup);
		const video::SColor col = friendly ? video::SColor(255, 0, 255, 0) : video::SColor(255, 255, 50, 50);
		driver->draw2DRectangle(col, core::recti(bx - 2, by - 2, bx + 2, by + 2));
	}

	// ---------- Lead indicator: where to aim so bullets intercept the moving target ----------
	SpaceObject* target = me->GetHardTarget() ? me->GetHardTarget() : me->GetSoftTarget();
	if(target && target->GetRenderObject())
	{
		const float projectileSpeed = 10000.0f;//muzzle speed of the machine-gun bullet
		const vector3df targetPos = target->GetRenderObject()->GetPosition();
		const vector3df d = targetPos - myPos;
		const vector3df v = m_EstimatedTargetVel;

		//Solve |d + v*t| = projectileSpeed*t for the earliest positive intercept time t.
		const float a = v.dotProduct(v) - projectileSpeed * projectileSpeed;
		const float b = 2.0f * d.dotProduct(v);
		const float c = d.dotProduct(d);

		float t = -1.0f;
		if(fabsf(a) < 0.0001f)
		{
			if(fabsf(b) > 0.0001f) t = -c / b;
		}
		else
		{
			const float disc = b * b - 4.0f * a * c;
			if(disc >= 0.0f)
			{
				const float sq = sqrtf(disc);
				const float t1 = (-b + sq) / (2.0f * a);
				const float t2 = (-b - sq) / (2.0f * a);
				if(t1 > 0.0f && t2 > 0.0f) t = std::min(t1, t2);
				else if(t1 > 0.0f) t = t1;
				else if(t2 > 0.0f) t = t2;
			}
		}

		if(t > 0.0f)
		{
			const vector3df intercept = targetPos + v * t;
			const vector3df camPos = cam->getAbsolutePosition();
			vector3df camFwd = cam->getTarget() - camPos;
			camFwd.normalize();
			if((intercept - camPos).dotProduct(camFwd) > 0.0f)//only if in front of the camera
			{
				const core::position2di sp = smgr->getSceneCollisionManager()->getScreenCoordinatesFrom3DPosition(intercept, cam);
				const video::SColor leadCol(255, 255, 180, 0);
				driver->draw2DRectangle(leadCol, core::recti(sp.X - 1, sp.Y - 9, sp.X + 1, sp.Y + 9));
				driver->draw2DRectangle(leadCol, core::recti(sp.X - 9, sp.Y - 1, sp.X + 9, sp.Y + 1));
			}
		}
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
