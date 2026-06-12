#pragma once
#include <winsock2.h>

#include "GUIView.h"

class MenuScene;
class GameListener;

namespace irr
{
	namespace scene
	{
		class IGUITable;
		class IGUIButton;
	}
}

class LANGUIView : public GUIView
{
	friend class MenuScene;

public:
	LANGUIView(MenuScene* parent);
	~LANGUIView();

	void show();
	void hide();

	void registerNewGame(wchar_t* gameName, wchar_t* ip, const unsigned int numberPlayers);
	void unregisterNewGame(wchar_t* gameName, wchar_t* ip);

	void stopListening();

	int connect(const wchar_t* gameIP, const wchar_t* playerName);

	SOCKET getSocket() const;

private:
	
	SOCKET m_socket;

	irr::gui::IGUITable* m_gameTable;
	irr::gui::IGUIButton* m_joinButton;

	GameListener* m_listener;
};

