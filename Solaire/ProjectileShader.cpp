#include <irrlicht.h>
#include "ProjectileShader.h"

using namespace irr;
using irr::core::vector3df;

ProjectileShader::ProjectileShader(const ShaderParams& params, scene::ISceneManager* smgr, PROJECTILE_TYPE type) :
				Shader(params), m_smgr(smgr)
{
	switch (type)
	{
	case PROJECTILE_EMP:
		m_Colour1 = vector3df(1.0f, 1.0f, 1.0f);
		m_Colour2 = vector3df(0.0f, 0.0f, 1.0f);
		break;
	case PROJECTILE_SLUG:
		m_Colour1 = vector3df(0.0f, 0.0f, 0.0f);
		m_Colour2 = vector3df(1.0f, 0.3f, 0.0f);
		break;
	case PROJECTILE_FLECHETTE:
		m_Colour1 = vector3df(0.8f, 0.8f, 0.8f);
		m_Colour2 = vector3df(0.5f, 0.5f, 0.5f);
		break;
	case PROJECTILE_BULLET:
		m_Colour1 = vector3df(1.0f, 1.0f, 1.0f);
		m_Colour2 = vector3df(0.6f, 0.6f, 0.0f);
		break;
	default:
		m_Colour1 = vector3df(1.0f, 1.0f, 1.0f);
		m_Colour2 = vector3df(0.6f, 0.6f, 0.0f);
		break;
	}
}

ProjectileShader::~ProjectileShader()
{
}

void ProjectileShader::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
    video::IVideoDriver* driver = services->getVideoDriver();
	
	core::matrix4 WorldMatrix = driver->getTransform(video::ETS_WORLD);	
	services->setVertexShaderConstant("matWorld", WorldMatrix.pointer(), 16);
	

	core::matrix4 WorldViewProjMatrix = driver->getTransform(video::ETS_PROJECTION);
	WorldViewProjMatrix *= driver->getTransform(video::ETS_VIEW);
	WorldViewProjMatrix *= driver->getTransform(video::ETS_WORLD);
	services->setVertexShaderConstant("matWorldViewProj", WorldViewProjMatrix.pointer(), 16);
	
	scene::ICameraSceneNode* cam = m_smgr->getActiveCamera();
	core::vector3df pos;
	if(cam)
	{
		pos = cam->getAbsolutePosition();
		services->setVertexShaderConstant("EyePos", reinterpret_cast<f32*>(&pos), 3);
	}
	
	services->setPixelShaderConstant("Colour1", reinterpret_cast<f32*>(&m_Colour1), 3);
	services->setPixelShaderConstant("Colour2", reinterpret_cast<f32*>(&m_Colour2), 3);

}