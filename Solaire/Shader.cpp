#include <cassert>

#include <irrlicht.h>

#include "Shader.h"

#include "System.h"

using namespace irr;

Shader::Shader(const ShaderParams& _params) : m_material(0), params(_params)
{
	validateTextures(params);
	m_material = System::get().getDevice()->getVideoDriver()->
		getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles(params.vertexShader.c_str(), params.vertexShaderMain.c_str(), video::EVST_VS_2_0, 
																		params.pixelShader.c_str(), params.pixelShaderMain.c_str(), video::EPST_PS_2_0, this, params.baseMaterial);
}

Shader::~Shader()
{
}

irr::s32 Shader::getMaterial() const
{
	return m_material;
}

void Shader::validateTextures(const ShaderParams& params)
{
	unsigned int p = 0;
	unsigned int indexZeroCount = 0;
	for(std::vector<TextureParam>::const_iterator it = params.textures.begin(); it != params.textures.end(); ++it)
	{
		TextureParam param = *it;
		if(param.index == 0)
		{
			if(indexZeroCount != 0)
			{
				core::stringw str = L"warning: there is already a texture at index 0 for shader '";
				str += params.pixelShader;
				str += "'";
				System::get().logw(str.c_str());
			}
			++indexZeroCount;
		}
		else
		{
			unsigned int next = param.index;
			if((p & next) == next)
			{
				core::stringw str = L"warning: there is already a texture at index ";
				str += next;
				str += " for shader '";
				str += params.pixelShader;
				str += "'";
				System::get().logw(str.c_str());
			}
			p |= next;
		}
	}
}

ShaderWrapper::ShaderWrapper(Shader* _shader, scene::ISceneNode* _node, void* _reserved) : shader(_shader), node(_node), reserved(_reserved)
{
	
	assert(shader != NULL);
	assert(node != NULL);
	


	node->setMaterialFlag(video::EMF_LIGHTING, shader->params.enableLighting);		
	node->getMaterial(0).NormalizeNormals = shader->params.normalizeNormals;
	
	for(std::vector<TextureParam>::const_iterator it = shader->params.textures.begin(); it != shader->params.textures.end(); ++it)
	{
		TextureParam tex = *it;
		node->setMaterialTexture(tex.index, System::get().getDevice()->getVideoDriver()->getTexture(tex.name));
	}
	node->setMaterialType((video::E_MATERIAL_TYPE)shader->getMaterial());
}

ShaderWrapper::~ShaderWrapper()
{

}
