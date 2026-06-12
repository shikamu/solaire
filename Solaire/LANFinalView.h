#pragma once
#include <vector>
#include <irrString.h>
#include "GUIView.h"

namespace irr
{
	namespace gui
	{
		class IGUIElement;
	}
}

class LANFinalView : public GUIView
{
public:
	LANFinalView(MenuScene* parent, const wchar_t* playerName);
	~LANFinalView();

	const wchar_t* getPlayerName() const;

	void updateNames(const std::vector<irr::core::stringw>& teamA, const std::vector<irr::core::stringw>& teamB);

	virtual void sendChatText(const wchar_t* playerName, const wchar_t* text);

	virtual void switchToLANScene() = 0;

protected:
	irr::core::stringw m_playerName;
	std::vector<irr::gui::IGUIElement*> m_names;

};

