#pragma once

#include "RenderObject.h"


class ProjectileRenderObject : public RenderObject
{
private: 
	const irr::io::path& GetPath(PROJECTILE_TYPE type);
public:
	ProjectileRenderObject();
	~ProjectileRenderObject();
	void Init(irr::scene::ISceneManager* smgr, SpaceObject* object, SpaceObject* host, PROJECTILE_TYPE type);
	void Init(irr::scene::ISceneManager* smgr, SpaceObject* object, vector3df& pos, vector3df& rot, PROJECTILE_TYPE type);
};