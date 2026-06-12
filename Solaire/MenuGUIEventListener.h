#pragma once
#include "IEventListener.h"

class MenuScene;

class MenuGUIEventListener : public IEventListener
{
public:
	MenuGUIEventListener(MenuScene* scene);
	~MenuGUIEventListener();

	bool onGUIEvent(const irr::SEvent::SGUIEvent& evt);

private:

	void generateErrorMessageBoxForJoinFailure(const int errorCode);

	void createLANNewGame();
	void joinLANGame();

	MenuScene* m_scene;
};

