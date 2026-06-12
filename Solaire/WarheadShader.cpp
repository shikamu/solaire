#include <irrlicht.h>

#include "WarheadShader.h"
#include "LogicConstants.h"
#include "RenderingConstants.h"

using namespace irr;

WarheadShader::WarheadShader(const ShaderParams& params, const unsigned int mask, scene::ISceneManager* smgr, irr::scene::ILightSceneNode* light) 
	: Shader(params), m_smgr(smgr), m_light(light)	
{
	if ((mask & MASK_GROUP_1) != 0)
	{
		m_MaskColour = COLOUR_GROUP1;
	}
	else if ((mask & MASK_GROUP_2) != 0)
	{
		m_MaskColour = COLOUR_GROUP2;
	}
	else if ((mask & MASK_GROUP_3) != 0)
	{
		m_MaskColour = COLOUR_GROUP3;
	}
	else if ((mask & MASK_GROUP_4) != 0)
	{
		m_MaskColour = COLOUR_GROUP4;
	}
	else m_MaskColour = COLOUR_DEFAULT;
}


WarheadShader::~WarheadShader()
{
}

void WarheadShader::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
    video::IVideoDriver* driver = services->getVideoDriver();
	
	core::matrix4 WorldMatrix = driver->getTransform(video::ETS_WORLD);	
	services->setVertexShaderConstant("matWorld", WorldMatrix.pointer(), 16);
	
	WorldMatrix = WorldMatrix.getTransposed();
	WorldMatrix.makeInverse();
	services->setVertexShaderConstant("matInvTransWorld", WorldMatrix.pointer(), 16);	

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
		pos = m_light->getAbsolutePosition();
		services->setVertexShaderConstant("LightPos", reinterpret_cast<f32*>(&pos), 3);
	}

	services->setPixelShaderConstant("MaskColour", reinterpret_cast<f32*>(&m_MaskColour), 3);
}
