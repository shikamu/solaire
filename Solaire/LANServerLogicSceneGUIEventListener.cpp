#include <cassert>
#include <irrlicht.h>

#include "LANServerLogicSceneGUIEventListener.h"
#include "GUIConstants.h"
#include "NetworkController.h"
#include "LANServer.h"
#include "System.h"

using namespace irr;


LANServerLogicSceneGUIEventListener::LANServerLogicSceneGUIEventListener()
{
}

LANServerLogicSceneGUIEventListener::~LANServerLogicSceneGUIEventListener()
{
}

bool LANServerLogicSceneGUIEventListener::onGUIEvent(const irr::SEvent::SGUIEvent& evt)
{
	s32 id = evt.Caller->getID();
	
	switch(evt.EventType)
	{
		case gui::EGET_BUTTON_CLICKED:
			switch(id)
			{
				case GUI_ID_GAMEMENU_CONTINUE_BUTTON:
				{
					return LogicSceneGUIEventListener::onGUIEvent(evt);
				}
				case GUI_ID_GAMEMENU_EXIT_BUTTON:
				{
					LANServer* server = NetworkController::get().getServer();
					assert(server != NULL);
					//NetworkController::get().unregisterLANServer();
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
