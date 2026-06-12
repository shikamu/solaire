#include <irrlicht.h>

#include "GUIConstants.h"
#include "MainMenuGUIView.h"
#include "System.h"
#include "RenderingConstants.h"	//TODO remove this

using namespace irr;

MainMenuGUIView::MainMenuGUIView(MenuScene* parent) : GUIView(parent)
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	//title
	float titleWidth = 0.9f;
	float titleHeight = titleWidth*0.25f;
	gui::IGUIImage* title = env->addImage(core::rect<s32>(static_cast<s32>(dim.Width*(1.0f-titleWidth)), static_cast<s32>(dim.Height*0.025f), static_cast<s32>(dim.Width*(titleWidth)), static_cast<s32>(dim.Height*(0.025f+titleHeight))));
	title->setImage(driver->getTexture("Title.tga"));
	title->setScaleImage(true);
	m_elements.push_back(title);

	//main menu stuff
	float buttonWidth = 0.1875f;
	float buttonHeight = 0.08f;
	//float x = (1.0f-buttonWidth)*0.5;
	float x = 0.1f;
	float y = 0.35f;
	float buttonDy = 0.125f;
						
	gui::IGUIButton* single = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*y), static_cast<s32>(dim.Width*(x+buttonWidth)), static_cast<s32>(dim.Height*(y+buttonHeight))), 0, GUI_ID_MAINMENU_SINGLE_BUTTON, L"SINGLE", L"Starts the game in single player");
	single->setImage(driver->getTexture("TestButtonUp.tga"));
	single->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	single->setScaleImage(true);
	single->setUseAlphaChannel(true);
	single->setDrawBorder(false);
	m_elements.push_back(single);

	gui::IGUIButton* lan = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*(y+buttonDy)), static_cast<s32>(dim.Width*(x+buttonWidth)), static_cast<s32>(dim.Height*(y+buttonHeight+buttonDy))), 0, GUI_ID_MAINMENU_LAN_BUTTON, L"LAN", L"Starts the game in multiplayer");
	lan->setImage(driver->getTexture("TestButtonUp.tga"));
	lan->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	lan->setScaleImage(true);
	lan->setUseAlphaChannel(true);
	lan->setDrawBorder(false);
	m_elements.push_back(lan);

	gui::IGUIButton* options = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*(y+2*buttonDy)), static_cast<s32>(dim.Width*(x+buttonWidth)), static_cast<s32>(dim.Height*(y+buttonHeight+2*buttonDy))), 0, GUI_ID_MAINMENU_OPTIONS_BUTTON, L"OPTIONS", L"Opens up the options menu");
	options->setImage(driver->getTexture("TestButtonUp.tga"));
	options->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	options->setScaleImage(true);
	options->setUseAlphaChannel(true);
	options->setDrawBorder(false);
	m_elements.push_back(options);

	gui::IGUIButton* exit = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x), static_cast<s32>(dim.Height*(y+3*buttonDy)), static_cast<s32>(dim.Width*(x+buttonWidth)), static_cast<s32>(dim.Height*(y+buttonHeight+3*buttonDy))), 0, GUI_ID_MAINMENU_EXIT_BUTTON, L"EXIT", L"Exits the game");
	exit->setImage(driver->getTexture("TestButtonUp.tga"));
	exit->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	exit->setScaleImage(true);
	exit->setUseAlphaChannel(true);
	exit->setDrawBorder(false);
	m_elements.push_back(exit);




	//gui::IGUITab* scoreboard = env->addTab(core::rect<s32>(static_cast<s32>(dim.Width*0.1f), static_cast<s32>(dim.Height*0.1f), static_cast<s32>(dim.Width*0.9f), static_cast<s32>(dim.Height*0.9f)));
	//scoreboard->setDrawBackground(true);
	//scoreboard->setBackgroundColor(video::SColor(128, 120, 120, 120));
	//m_elements.push_back(scoreboard);

	//core::dimension2d<s32> scoreDim = scoreboard->getAbsoluteClippingRect().getSize();

	//wchar_t* headers[4] = {L"Name", L"Score", L"Kills", L"Deaths"};
	//float columns[4][4] = {
	//	{0.025f, 0.025f, 0.3f, 0.035f},		//column1: X, Y, Width, Height
	//	{0.0f, 0.025f, 0.2f, 0.035f},			//column2: X, Y, Width, Height
	//	{0.0f, 0.025f, 0.2f, 0.035f},			//column3: X, Y, Width, Height
	//	{0.0f, 0.025f, 0.2f, 0.035f},			//column4: X, Y, Width, Height
	//};

	//static const float dx = 0.025f;
	//float xStart = 0.0f;

	//float yStart = 0.0f;//this is for the players list
	////header
	//for(unsigned int i = 0; i < 4; ++i)
	//{
	//	env->addStaticText(headers[i], 
	//		core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[i][0])), static_cast<s32>(scoreDim.Height*columns[i][1]), static_cast<s32>(scoreDim.Width*(xStart+columns[i][0]+columns[i][2])), static_cast<s32>(scoreDim.Height*(columns[i][1]+columns[i][3]))), 
	//		false, false, scoreboard);
	//	xStart+= columns[i][0] + columns[i][2]+dx;

	//	yStart = std::max(yStart, columns[i][1]+columns[i][3]);
	//}

	////temporary, just because i don't want to include LogicConstants here because it's just a test
	//static const unsigned int MASK_GROUP_1 = 1 << 16;
	//static const unsigned int MASK_GROUP_2 = 1 << 17;
	//static const unsigned int MASK_GROUP_3 = 1 << 18;
	//static const unsigned int MASK_GROUP_4 = 1 << 19;



	//wchar_t* players[6] = {L"Thomas", L"Matt", L"Karl", L"B-rad", L"Someone", L"N00B"};
	//unsigned int values[6][4] = {
	//	{120, 6, 0, MASK_GROUP_1},				//score, kills, deaths, team for players[0]
	//	{100, 5, 3, MASK_GROUP_2},				//score, kills, deaths, team for players[1]
	//	{90, 4, 1, MASK_GROUP_3},				//score, kills, deaths, team for players[2]
	//	{70, 3, 5, MASK_GROUP_1},				//score, kills, deaths, team for players[3]
	//	{55, 2, 2, MASK_GROUP_4},				//score, kills, deaths, team for players[4]
	//	{37, 1, 4, MASK_GROUP_2},				//score, kills, deaths, team for players[5]
	//};

	//
	//static const float dy = 0.04f;
	//yStart+=dy;
	//for(unsigned int i = 0; i < 6; ++i)
	//{
	//	xStart = 0.0f;
	//	unsigned int mask = values[i][3];
	//	core::vector3df m_MaskColour;
	//	if ((mask & MASK_GROUP_1) != 0)
	//	{
	//		m_MaskColour = COLOUR_GROUP1;
	//	}
	//	else if ((mask & MASK_GROUP_2) != 0)
	//	{
	//		m_MaskColour = COLOUR_GROUP2;
	//	}
	//	else if ((mask & MASK_GROUP_3) != 0)
	//	{
	//		m_MaskColour = COLOUR_GROUP3;
	//	}
	//	else if ((mask & MASK_GROUP_4) != 0)
	//	{
	//		m_MaskColour = COLOUR_GROUP4;
	//	}
	//	else m_MaskColour = COLOUR_DEFAULT;


	//	for(unsigned int j = 0; j < 4; ++j)
	//	{
	//		core::stringw text;
	//		if(j == 0)
	//			text = players[i];
	//		else
	//		{
	//			text = core::stringw(values[i][j-1]);
	//		}
	//		gui::IGUIStaticText* label = env->addStaticText(text.c_str(), 
	//			core::rect<s32>(static_cast<s32>(scoreDim.Width*(xStart+columns[j][0])), static_cast<s32>(scoreDim.Height*(yStart+columns[j][1])), static_cast<s32>(scoreDim.Width*(xStart+columns[j][0]+columns[j][2])), static_cast<s32>(scoreDim.Height*(yStart+columns[j][1]+columns[j][3]))), 
	//			false, false, scoreboard);
	//		label->setOverrideColor(video::SColor(255, 255*m_MaskColour.X, 255*m_MaskColour.Y, 255*m_MaskColour.Z));

	//		xStart+= columns[j][0] + columns[j][2]+dx;
	//	}
	//	yStart+=dy;
	//}
}

MainMenuGUIView::~MainMenuGUIView()
{
	hide();
}
