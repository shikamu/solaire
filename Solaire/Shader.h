#pragma once
#include <vector>
#include <string>

#include <EMaterialTypes.h>
#include <irrString.h>
#include <IShaderConstantSetCallBack.h>

namespace irr
{
	namespace scene
	{
		class ISceneNode;
	}
}

class SpaceObject;


struct TextureParam
{
	unsigned int index;
	irr::core::stringw name;
	TextureParam(const unsigned int idx, const irr::core::stringw& n) : index(idx), name(n){}
	~TextureParam(){}
};

struct ShaderParams
{
	irr::core::stringw vertexShader;
	irr::core::stringw pixelShader;

	std::string vertexShaderMain;
	std::string pixelShaderMain;

	irr::video::E_MATERIAL_TYPE baseMaterial;

	std::vector<TextureParam> textures;
	SpaceObject* spaceobject;
	bool normalizeNormals;
	bool enableLighting;

	ShaderParams() : vertexShaderMain("vs_main"), pixelShaderMain("ps_main"), baseMaterial(irr::video::EMT_TRANSPARENT_ALPHA_CHANNEL), spaceobject(NULL), normalizeNormals(true), enableLighting(false){}
	~ShaderParams(){}
};

class Shader : public irr::video::IShaderConstantSetCallBack
{
public:
	Shader(const ShaderParams& params);
	virtual ~Shader();

	virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) = 0;

	ShaderParams params;

	irr::s32 getMaterial() const;

private:

	void validateTextures(const ShaderParams& params);
	irr::s32 m_material;

};

class ShaderWrapper
{
public:

	ShaderWrapper(Shader* _shader, irr::scene::ISceneNode* _node, void* _reserved);
	~ShaderWrapper();

	Shader* shader;
	irr::scene::ISceneNode* node;
	void* reserved;
};

