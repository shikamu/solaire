#include <irrlicht.h>
#include "StandardShader.h"

using namespace irr;

StandardShader::StandardShader(const ShaderParams& params, scene::ISceneManager* smgr, irr::scene::ILightSceneNode* light) :
				Shader(params), m_smgr(smgr), m_light(light)
{
}

StandardShader::~StandardShader()
{
}

void StandardShader::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
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

	if(m_light)
	{
		pos = m_light->getPosition();
		services->setVertexShaderConstant("LightPos", reinterpret_cast<f32*>(&pos), 3);
	}
}