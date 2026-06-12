#include <irrlicht.h>

#include "LockShader.h"

using namespace irr;

LockShader::LockShader(const ShaderParams& params, irr::core::vector3df* topLeft, irr::core::vector3df* bottomRight, irr::core::vector3df* direction, float* visibility) 
	: Shader(params), m_TopLeft(topLeft), m_BottomRight(bottomRight), m_Direction(direction), m_Visibility(visibility)
{
}

LockShader::~LockShader()
{
}

void LockShader::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
	video::IVideoDriver* driver = services->getVideoDriver();

	core::matrix4 ProjMatrix =  driver->getTransform(video::ETS_PROJECTION);
	services->setVertexShaderConstant("matProj", ProjMatrix.pointer(), 16);

	
	services->setVertexShaderConstant("TargetDirection",reinterpret_cast<f32*>(m_Direction), 3);
	//services->setVertexShaderConstant("TopLeft",reinterpret_cast<f32*>(m_TopLeft), 3);
	//services->setVertexShaderConstant("BottomRight",reinterpret_cast<f32*>(m_BottomRight), 3);
	services->setPixelShaderConstant("Visibility", m_Visibility, 1);

}
