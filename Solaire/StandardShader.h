#pragma once
#include "Shader.h"

namespace irr
{
	namespace scene
	{
		class ISceneManager;
		class ILightSceneNode;
	};
};

class StandardShader : public Shader
{
public:
	StandardShader(const ShaderParams& params, irr::scene::ISceneManager* smgr, irr::scene::ILightSceneNode* light);
	~StandardShader();

	void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData);

private:
	irr::scene::ISceneManager* m_smgr;
	irr::scene::ILightSceneNode* m_light;
};