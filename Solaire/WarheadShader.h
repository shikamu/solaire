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

class WarheadShader : public Shader
{
public:
	WarheadShader(const ShaderParams& params, const unsigned int mask, irr::scene::ISceneManager* smgr, irr::scene::ILightSceneNode* light);
	~WarheadShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:
	irr::scene::ISceneManager* m_smgr;
	irr::scene::ILightSceneNode* m_light;
	irr::core::vector3df m_MaskColour;
};
