#pragma once
#include "Shader.h"
#include "FactoryConstants.h"

namespace irr
{
	namespace scene
	{
		class ISceneManager;
	};
};

class ProjectileShader : public Shader
{
public:
	ProjectileShader(const ShaderParams& params, irr::scene::ISceneManager* smgr, PROJECTILE_TYPE type);
	~ProjectileShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:
	irr::scene::ISceneManager* m_smgr;
	irr::core::vector3df m_Colour1; 
	irr::core::vector3df m_Colour2; 
};