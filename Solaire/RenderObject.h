#pragma once
#include <vector>
#include <vector3d.h>
#include "FactoryConstants.h"
using std::vector;
using irr::core::vector3df;

namespace irr
{
	namespace scene
	{
		class IMeshSceneNode;
		class ISceneManager;
		class IParticleSystemSceneNode;
	}
}

class Shader;
class SpaceObject;

class RenderObject
{
protected:
	irr::scene::IMeshSceneNode* m_RootNode; 

	int m_outlineShaderMaterial;
	float m_outlineShaderTime;

	vector<Shader*> m_ShaderList; 
	void StoreShader(Shader* in);

public:
	RenderObject() : m_RootNode (NULL), m_outlineShaderMaterial(0), m_outlineShaderTime(1.0f) {} 
	virtual ~RenderObject();
	void SetSceneNode(irr::scene::IMeshSceneNode* node);
	irr::scene::IMeshSceneNode* GetSceneNode();

	void SetRotation(const vector3df &rot);
	const vector3df& GetRotation();
	void SetPosition(const vector3df &pos);
	const vector3df& GetPosition();

	//objects that can be selected simply need to override this and the TargettingModule will handle the rest
	virtual irr::scene::IMeshSceneNode* getOutlineNode();

	virtual void Update(const float dt); 

	int getOutlineShaderMaterial() const
	{
		return m_outlineShaderMaterial;
	}

	void setOutlineShaderTime(const float f)
	{
		m_outlineShaderTime = f;
	}

private:
};

class ShipRenderObject : public RenderObject
{
public:
	ShipRenderObject() : m_outlineNode(NULL) {}
	~ShipRenderObject() {} 
	void Init(irr::scene::ISceneManager* smgr, SpaceObject* object);

	irr::scene::IMeshSceneNode* getOutlineNode();
	void Update(const float dt);

private:
	irr::scene::IMeshSceneNode* m_outlineNode;
};

