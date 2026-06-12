#include <irrlicht.h>
#include "ProjectileRenderObject.h"
#include "ShaderIncludes.h"
#include "System.h"
#include "LogicConstants.h"
#include "SpaceObject.h"

ProjectileRenderObject::ProjectileRenderObject()
{

}
ProjectileRenderObject::~ProjectileRenderObject()
{

}

const irr::io::path& ProjectileRenderObject::GetPath(PROJECTILE_TYPE type)
{
	switch (type)
	{
	case PROJECTILE_BULLET:
		return FILE_PROJECTILE_BULLET;
		break;
	case PROJECTILE_EMP:
		return FILE_PROJECTILE_EMP;
		break;
	case PROJECTILE_SLUG:
		return FILE_PROJECTILE_SLUG;
		break;
	case PROJECTILE_FLECHETTE:
		return FILE_PROJECTILE_FLECHETTE;
		break;
	default:
		return FILE_PROJECTILE_BULLET;
		break;
	}
}

void ProjectileRenderObject::Init(irr::scene::ISceneManager* smgr, SpaceObject* object, SpaceObject* host, PROJECTILE_TYPE type)
{
	ShaderParams params1;
	params1.vertexShader = L"ProjectileShader.hlsl";
	params1.pixelShader = L"ProjectileShader.hlsl";
	ProjectileShader* m_bulletshader = new ProjectileShader(params1, smgr, type);
	StoreShader(m_bulletshader);

	irr::scene::IMeshSceneNode* m_bulletNode = smgr->addMeshSceneNode(smgr->getMesh(GetPath(type)));

	if(m_bulletNode)
	{
		m_bulletNode->setPosition(host->GetRenderObject()->GetPosition());
		m_bulletNode->setRotation(host->GetRenderObject()->GetRotation());
		ShaderWrapper shader1(m_bulletshader, m_bulletNode, NULL);		
	}
	SetSceneNode(m_bulletNode);
	//std::cout << "SO id=" << ID << ", sceneNode=" << m_bulletNode << std::endl;
}

void ProjectileRenderObject::Init(irr::scene::ISceneManager* smgr, SpaceObject* object, vector3df& pos, vector3df& rot, PROJECTILE_TYPE type)
{
	ShaderParams params1;
	params1.vertexShader = L"ProjectileShader.hlsl";
	params1.pixelShader = L"ProjectileShader.hlsl";
	ProjectileShader* m_bulletshader = new ProjectileShader(params1, smgr, type);
	StoreShader(m_bulletshader);
	
	irr::scene::IMeshSceneNode* m_bulletNode = smgr->addMeshSceneNode(smgr->getMesh(GetPath(type)));

	if(m_bulletNode)
	{
		m_bulletNode->setPosition(pos);
		m_bulletNode->setRotation(rot);
		ShaderWrapper shader1(m_bulletshader, m_bulletNode, NULL);		
	}
	SetSceneNode(m_bulletNode);
	//std::cout << "SO id=" << ID << ", sceneNode=" << m_bulletNode << std::endl;
}