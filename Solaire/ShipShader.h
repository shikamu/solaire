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

class ShipShader : public Shader
{
public:
	ShipShader(const ShaderParams& params, const unsigned int mask, irr::scene::ISceneManager* smgr, irr::scene::ILightSceneNode* light, float* remaining, float* time, vector3df* direction);
	~ShipShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:
	irr::scene::ISceneManager* m_smgr;
	irr::scene::ILightSceneNode* m_light;
	float* m_Remaining;
	float* m_ImpactTime;
	irr::core::vector3df* m_Direction; 
	irr::core::vector3df m_MaskColour;
};

