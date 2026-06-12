#pragma once

#include "RenderObject.h"


class WarheadRenderObject : public RenderObject
{
private: 
	const irr::io::path& GetPath(WARHEAD_TYPE type);
public:
	WarheadRenderObject();
	~WarheadRenderObject();
	void Init(irr::scene::ISceneManager* smgr, SpaceObject* object, SpaceObject* host, WARHEAD_TYPE type);
	void Init(irr::scene::ISceneManager* smgr, SpaceObject* object, vector3df& pos, vector3df& rot, WARHEAD_TYPE type);
};