#include <irrlicht.h>

#include "RenderManager.h"

#include "System.h"

using namespace irr;

RenderManager RenderManager::m_RenderManager;

RenderManager::RenderManager()
{

}

RenderManager::~RenderManager()
{

}

RenderManager& RenderManager::get()
{
	return m_RenderManager;
}

void RenderManager::render()
{
	System::get().getDevice()->getVideoDriver()->beginScene(true, true, video::SColor(0,0,0,0));
	
	irr::video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();

	scene::ISceneManager* smgr = System::get().getCurrentSceneManager();
	if(smgr)
	{
		smgr->drawAll();
	}
	//System::get().getCurrentSceneManager()->drawAll();
	System::get().getDevice()->getGUIEnvironment()->drawAll();
	System::get().getDevice()->getVideoDriver()->endScene();

	wchar_t msg[128];
	//io::IAttributes* attr = m_smgr->getParameters();
	if(smgr)
	{
		io::IAttributes* attr = smgr->getParameters();

		swprintf(msg, 128,
			L"Sol@ire v1.3 %ls [%ls], FPS:%03d Tri:%.03fm Cull %d/%d nodes (%d,%d,%d)",
#ifdef _DEBUG
			L"DEBUG",
#else
			L"RELEASE",
#endif
			System::get().getDevice()->getVideoDriver()->getName(),
			System::get().getDevice()->getVideoDriver()->getFPS(),
			(f32)System::get().getDevice()->getVideoDriver()->getPrimitiveCountDrawn(0)*(1.0f*0.000001f),
			attr->getAttributeAsInt("culled"),
			attr->getAttributeAsInt("calls"),
			attr->getAttributeAsInt("drawn_solid"),
			attr->getAttributeAsInt("drawn_transparent"),
			attr->getAttributeAsInt("drawn_transparent_effect")
		);
		System::get().getDevice()->setWindowCaption(msg);
		if (!smgr->getActiveCamera())
		{
			int i = 3;
			++i;
		}
	}
}
