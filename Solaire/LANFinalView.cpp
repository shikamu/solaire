#include <algorithm>
#include <irrlicht.h>

#include "LANFinalView.h"
#include "System.h"
#include "GUIConstants.h"

using namespace irr;


LANFinalView::LANFinalView(MenuScene* parent, const wchar_t* playerName) : GUIView(parent), m_playerName(playerName)
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	float x = 0.3f;
	float y = 0.35f;
	float w = 0.1f;
	gui::IGUIStaticText* teamA = env->addStaticText(L"Team A", core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(x+w)), static_cast<s32>(dim.Height*(y+0.05f))));
	teamA->setOverrideColor(video::SColor(255, 255, 255, 255));	
	m_elements.push_back(teamA);

	float x2 = 0.7f;
	gui::IGUIStaticText* teamB = env->addStaticText(L"Team B", core::rect<s32>(static_cast<s32>(dim.Width*x2), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(x2+w)), static_cast<s32>(dim.Height*(y+0.05f))));
	teamB->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_elements.push_back(teamB);

	float chatX = x+w+0.025f;
	float chatW = x2-0.025f;
	float chatH = 0.2f;

	gui::IGUIEditBox* chat = env->addEditBox(L"", core::rect<s32>(static_cast<s32>(dim.Width*(chatX)), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(chatW)), static_cast<s32>(dim.Height*(y+chatH))), false, NULL, GUI_ID_LANFINAL_CHAT_EDITBOX);
	chat->setMultiLine(true);
	chat->setWordWrap(true);
	chat->setOverrideColor(video::SColor(255, 255, 255, 255));
	chat->setOverrideFont(env->getFont("courrier8.xml"));
	chat->setEnabled(false);
	chat->setAutoScroll(true);	
	m_elements.push_back(chat);

	gui::IGUIEditBox* chatInput = env->addEditBox(L"", core::rect<s32>(static_cast<s32>(dim.Width*(chatX)), static_cast<s32>(dim.Height*(y+chatH+0.05f)), static_cast<s32>(dim.Width*(chatW)), static_cast<s32>(dim.Height*(y+chatH+0.05f)+25)), false, NULL, GUI_ID_LANFINAL_INPUTCHAT_EDITBOX);
	chatInput->setOverrideColor(video::SColor(255, 0, 0, 0));
	chatInput->setOverrideFont(env->getFont("courrier8.xml"));
	chatInput->setDrawBorder(true);	
	env->setFocus(chatInput);
	m_elements.push_back(chatInput);


	hide();
}

LANFinalView::~LANFinalView()
{
	hide();
}

const wchar_t* LANFinalView::getPlayerName() const
{
	return m_playerName.c_str();
}

void LANFinalView::updateNames(const std::vector<irr::core::stringw>& teamA, const std::vector<irr::core::stringw>& teamB)
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	float x = 0.3f;
	float y = 0.4f;
	float w = 0.1f;

	for(std::vector<irr::gui::IGUIElement*>::const_iterator it = m_names.begin(); it != m_names.end(); ++it)
	{
		gui::IGUIElement* elt = *it;
		(*it)->remove();
		m_elements.erase(std::remove(m_elements.begin(), m_elements.end(), elt), m_elements.end());
		/*
		gui::IGUIElement* elt = *it;
		elt->setVisible(false);
		System::get().getDevice()->getGUIEnvironment()->getRootGUIElement()->removeChild(elt);
		m_elements.erase(std::remove(m_elements.begin(), m_elements.end(), elt), m_elements.end());
		*/
	}
	m_names.clear();

	float dy = 0.0f;
	for(std::vector<core::stringw>::const_iterator it = teamA.begin(); it != teamA.end(); ++it)
	{
		gui::IGUIStaticText* name = env->addStaticText((*it).c_str(), core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*(y+dy)), static_cast<s32>(dim.Width*(x+w)), static_cast<s32>(dim.Height*(y+0.05f+dy))));
		name->setOverrideColor(video::SColor(255, 255, 255, 255));	
		m_elements.push_back(name);
		m_names.push_back(name);
		dy+=0.05f;
	}

	dy = 0.0f;
	x = 0.7f;
	for(std::vector<core::stringw>::const_iterator it = teamB.begin(); it != teamB.end(); ++it)
	{
		gui::IGUIStaticText* name = env->addStaticText((*it).c_str(), core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*(y+dy)), static_cast<s32>(dim.Width*(x+w)), static_cast<s32>(dim.Height*(y+0.05f+dy))));
		name->setOverrideColor(video::SColor(255, 255, 255, 255));	
		m_elements.push_back(name);
		m_names.push_back(name);
		dy+=0.05f;
	}
}

void LANFinalView::sendChatText(const wchar_t* playerName, const wchar_t* text)
{
	//Route the local echo through the same queued, trimmed path as everything else so the
	//chat box stays consistent and doesn't overflow.
	core::stringw line(playerName);
	line += L": ";
	line += text;
	System::get().appendLobbyChatLine(line.c_str());
}