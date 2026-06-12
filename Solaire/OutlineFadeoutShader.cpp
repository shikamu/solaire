#include <irrlicht.h>

#include "OutlineFadeoutShader.h"

using namespace irr;

OutlineFadeoutShader::OutlineFadeoutShader(const ShaderParams& params, float* time) 
	: Shader(params), m_time(time)
{
}

OutlineFadeoutShader::~OutlineFadeoutShader()
{
}

void OutlineFadeoutShader::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32)
{
    video::IVideoDriver* driver = services->getVideoDriver();

	core::matrix4 WorldViewProjMatrix = driver->getTransform(video::ETS_PROJECTION);
	WorldViewProjMatrix *= driver->getTransform(video::ETS_VIEW);
	WorldViewProjMatrix *= driver->getTransform(video::ETS_WORLD);
	services->setVertexShaderConstant("matWorldViewProj", WorldViewProjMatrix.pointer(), 16);

	services->setPixelShaderConstant("Time", m_time, 1);
}
