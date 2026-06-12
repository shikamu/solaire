#include <cassert>
#include <irrlicht.h>

#include "LANClientLogicSceneGUIEventListener.h"
#include "GUIConstants.h"
#include "NetworkController.h"
#include "LANClient.h"

using namespace irr;


LANClientLogicSceneGUIEventListener::LANClientLogicSceneGUIEventListener()
{
}

LANClientLogicSceneGUIEventListener::~LANClientLogicSceneGUIEventListener()
{
}

bool LANClientLogicSceneGUIEventListener::onGUIEvent(const irr::SEvent::SGUIEvent& evt)
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
					LANClient* client = NetworkController::get().getClient();
					assert(client != NULL);
					client->backToLANView();

					return true;
				}
				default:
					return false;
			}
			break;
		default:
			break;
	}
	return false;}
