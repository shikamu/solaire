#pragma once
#include "Shader.h"

class HUDShader : public Shader
{
public:
	HUDShader(const ShaderParams& params, float* const shield, float * const armour);
	~HUDShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:

	float* const m_shield;
	float* const m_armour;

};
