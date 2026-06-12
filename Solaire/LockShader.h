#pragma once
#include <vector3d.h>
#include "Shader.h"

class LockShader : public Shader
{
public:
	LockShader(const ShaderParams& params, irr::core::vector3df* topLeft, irr::core::vector3df* bottomRight, irr::core::vector3df* direction, float* visibility);
	~LockShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:

	irr::core::vector3df* m_TopLeft;
	irr::core::vector3df* m_BottomRight;
	irr::core::vector3df* m_Direction;
	float* m_Visibility;

};
