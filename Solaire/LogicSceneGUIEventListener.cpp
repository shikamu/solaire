#include <irrlicht.h>

#include "LogicSceneGUIEventListener.h"
#include "System.h"
#include "GUIConstants.h"

using namespace irr;

LogicSceneGUIEventListener::LogicSceneGUIEventListener()
{
}

LogicSceneGUIEventListener::~LogicSceneGUIEventListener()
{
}

bool LogicSceneGUIEventListener::onGUIEvent(const irr::SEvent::SGUIEvent& evt)
{
	s32 id = evt.Caller->getID();
	
	switch(evt.EventType)
	{
		case gui::EGET_BUTTON_CLICKED:
			switch(id)
			{
				case GUI_ID_GAMEMENU_CONTINUE_BUTTON:
				{
					gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
					gui::IGUIElement* container = env->getRootGUIElement()->getElementFromId(GUI_ID_GAMEMENU_CONTAINER, false);
					if(container)
					{
						container->setVisible(false);
					}
					return true;
				}
				case GUI_ID_GAMEMENU_EXIT_BUTTON:
				{
					gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
					/*
					gui::IGUIElement* container = env->getRootGUIElement()->getElementFromId(GUI_ID_GAMEMENU_CONTAINER, false);
					if(container)
					{
						container->setVisible(false);
					}
					*/
					System::get().toggle(SCENE_MENU);
					return true;
				}
				default:
					return false;
			}
			break;
		default:
			break;
	}
	return false;
}
