#pragma once
#include "Shader.h"
#include <vector3d.h>

using irr::core::vector3df;

namespace irr
{
	namespace scene
	{
		class ISceneManager;
		class ILightSceneNode;
	}
}

class OutlineFadeoutShader : public Shader
{
public:
	OutlineFadeoutShader(const ShaderParams& params, float* time);
	~OutlineFadeoutShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:
	float* m_time;
};

