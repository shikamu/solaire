#include <irrlicht.h>

#include "HUDShader.h"

using namespace irr;

HUDShader::HUDShader(const ShaderParams& params, float* const shield, float * const armour) : Shader(params), m_shield(shield), m_armour(armour)
{
}

HUDShader::~HUDShader()
{
}

void HUDShader::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
	video::IVideoDriver* driver = services->getVideoDriver();

	core::matrix4 ProjMatrix =  driver->getTransform(video::ETS_PROJECTION);
	services->setVertexShaderConstant("matProj", ProjMatrix.pointer(), 16);

	services->setPixelShaderConstant("Shield", m_shield, 1);
	services->setPixelShaderConstant("Armour", m_armour, 1);
}
