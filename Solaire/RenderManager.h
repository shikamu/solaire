#pragma once
#include "AutoCamera.h"

class RenderManager
{
public:

	static RenderManager& get();
	void render();
	AutoCamera& GetCamera()
	{
		return m_Camera;
	}
private:
	
	RenderManager();
	~RenderManager();

	static RenderManager m_RenderManager;
	AutoCamera m_Camera; 
};

