#include <irrlicht.h>

#include "SinglePlayerGUIView.h"
#include "System.h"
#include "GUIViewAnimator.h"
#include "GUIConstants.h"
#include "MenuScene.h"
#include "Shader.h"
#include "ShaderIncludes.h"

using namespace irr;


SinglePlayerGUIView::SinglePlayerGUIView(MenuScene* parent) : GUIView(parent), m_animator(NULL), m_leftChevron(NULL), m_rightChevron(NULL), m_play(NULL), m_camera(NULL), m_shipNode(NULL), m_shipShader1(NULL), m_shipShader2(NULL),
	m_currentShip(0),
	m_chevronWidth(0.1f), m_chevronHeight(0.1f), m_leftChevronX(0.4f), m_leftChevronY(0.35f), m_rightChevronX(0.85f), m_rightChevronY(m_leftChevronY), m_playWidth(0.1875f)
{
	

	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();


	m_leftChevron = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*m_leftChevronX), static_cast<s32>(dim.Height*m_leftChevronY), static_cast<s32>(dim.Width*(m_leftChevronX+m_chevronWidth)), static_cast<s32>(dim.Height*(m_leftChevronY+m_chevronHeight))), 0, GUI_ID_SINGLEMENU_PREVIOUS_BUTTON, L"", L"previous ship");
	m_leftChevron->setImage(driver->getTexture("LeftArrowUp.tga"));
	m_leftChevron->setPressedImage(driver->getTexture("LeftArrowDown.tga"));
	m_leftChevron->setScaleImage(true);
	m_leftChevron->setUseAlphaChannel(true);
	m_leftChevron->setDrawBorder(false);
	m_elements.push_back(m_leftChevron);

	m_rightChevron = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*m_rightChevronX), static_cast<s32>(dim.Height*m_rightChevronY), static_cast<s32>(dim.Width*(m_rightChevronX+m_chevronWidth)), static_cast<s32>(dim.Height*(m_rightChevronY+m_chevronHeight))), 0, GUI_ID_SINGLEMENU_NEXT_BUTTON, L"", L"next ship");
	m_rightChevron->setImage(driver->getTexture("RightArrowUp.tga"));
	m_rightChevron->setPressedImage(driver->getTexture("RightArrowDown.tga"));
	m_rightChevron->setScaleImage(true);
	m_rightChevron->setUseAlphaChannel(true);
	m_rightChevron->setDrawBorder(false);
	m_elements.push_back(m_rightChevron);

	m_play = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*(m_leftChevronX+(m_rightChevronX-m_leftChevronX+m_chevronWidth-m_playWidth)*0.5f)), static_cast<s32>(dim.Height*0.65f), static_cast<s32>(dim.Width*(m_leftChevronX+(m_rightChevronX-m_leftChevronX+m_chevronWidth-m_playWidth)*0.5f+m_playWidth)), static_cast<s32>(dim.Height*(0.65f+0.1f))), 0, GUI_ID_SINGLEMENU_PLAY_BUTTON, L"PLAY", L"Launches the game");
	m_play->setImage(driver->getTexture("TestButtonUp.tga"));
	m_play->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	m_play->setScaleImage(true);
	m_play->setUseAlphaChannel(true);
	m_play->setDrawBorder(false);
	m_elements.push_back(m_play);


	m_shipNode = m_parent->getSceneManager()->addEmptySceneNode();
	
	if(m_shipNode)
	{
		bool success = true;
		scene::IAnimatedMeshSceneNode* m_ship1 = m_parent->getSceneManager()->addAnimatedMeshSceneNode(m_parent->getSceneManager()->getMesh("Fighter2.obj"), m_shipNode, 1);
		if(m_ship1)
		{
			m_ship1->setPosition(core::vector3df(15, 10, 0));
			m_ship1->setScale(core::vector3df(0.15f, 0.15f, 0.15f));
		}else success = false;
			
		scene::IMeshSceneNode* m_ship2 = m_parent->getSceneManager()->addMeshSceneNode(m_parent->getSceneManager()->getMesh("FighterHull.obj"), m_shipNode, 1);
		if(m_ship2)
		{
			m_ship2->setPosition(core::vector3df(15, 10, 0));
			m_ship2->setScale(core::vector3df(0.15f, 0.15f, 0.15f));
			m_ship2->setVisible(false);
		}else success = false;

		scene::ISceneNodeAnimator *anim = m_parent->getSceneManager()->createRotationAnimator(core::vector3df(0.0f, 0.45f, 0.225f));
		if (success && anim)
		{
			m_ship1->addAnimator(anim);
			m_ship2->addAnimator(anim);
			anim->drop();
		}

		m_camera = m_parent->getSceneManager()->addCameraSceneNode(0, core::vector3df(0.0f, 30.0f, -40.0f), core::vector3df(0.0f, 5.0f, 0.0f));
		scene::ILightSceneNode* light = m_parent->getSceneManager()->addLightSceneNode(0, core::vector3df(0.0f,100.0f,0.0f), video::SColorf(1.0f, 0.6f, 0.7f, 1.0f), 800.0f, 99);
		ShaderParams params1, params2;
		params1.vertexShader = L"StandardShader.hlsl";
		params1.pixelShader = L"StandardShader.hlsl";
		params1.textures.push_back(TextureParam(0, "FighterTexture.tga"));
		params1.textures.push_back(TextureParam(1, "FighterAlpha.tga"));

		params2.vertexShader = L"StandardShader.hlsl";
		params2.pixelShader = L"StandardShader.hlsl";
		params2.textures.push_back(TextureParam(0, "Fighter1UV.tga"));
		params2.textures.push_back(TextureParam(1, "Fighter1UVAlpha.tga"));
		m_shipShader1 = new StandardShader(params1, m_parent->getSceneManager(), light);
		m_shipShader2 = new StandardShader(params2, m_parent->getSceneManager(), light);
		ShaderWrapper shader1(m_shipShader1, m_ship1, NULL);
		ShaderWrapper shader2(m_shipShader2, m_ship2, NULL);
	}

	hide();
}

SinglePlayerGUIView::~SinglePlayerGUIView()
{
	hide();
	if(m_shipShader1)
	{
		delete m_shipShader1;
		m_shipShader1 = NULL;
	}

	if(m_shipShader2)
	{
		delete m_shipShader2;
		m_shipShader2 = NULL;
	}
}

void SinglePlayerGUIView::hide()
{
	if(m_animator)
	{
		//m_animator->interrupt();
		m_animator->finish();
		delete m_animator;
		m_animator = NULL;
	}

	m_shipNode->setVisible(false);
	GUIView::hide();
}

void SinglePlayerGUIView::show()
{
	if(m_animator)
	{
		//m_animator->interrupt();
		m_animator->finish();
		delete m_animator;
		m_animator = NULL;
	}
	core::dimension2d<u32> dim = System::get().getDevice()->getVideoDriver()->getScreenSize();

	m_animator = new GUIViewAnimator(250, System::get().getDevice());
	m_animator->addElement(new GUIAnimation(static_cast<s32>(dim.Width*m_leftChevronX), static_cast<s32>(dim.Height*m_leftChevronY), m_leftChevron));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(dim.Width*m_rightChevronX), static_cast<s32>(dim.Height*m_rightChevronY), m_rightChevron));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(dim.Width*(m_leftChevronX+(m_rightChevronX-m_leftChevronX+m_chevronWidth-m_playWidth)*0.5f)), static_cast<s32>(dim.Height*0.65f), m_play));

	m_shipNode->setVisible(true);
	GUIView::show();
	m_animator->start();
}

void SinglePlayerGUIView::toggleNextShip()
{
	u32 size = m_shipNode->getChildren().size();
	(*(m_shipNode->getChildren().begin()+m_currentShip))->setVisible(false);
	m_currentShip = ( m_currentShip + 1 ) % size;
	(*(m_shipNode->getChildren().begin()+m_currentShip))->setVisible(true);	
}

void SinglePlayerGUIView::togglePreviousShip()
{
	u32 size = m_shipNode->getChildren().size();
	(*(m_shipNode->getChildren().begin()+m_currentShip))->setVisible(false);
	if(m_currentShip == 0)
		m_currentShip = size-1;
	else
		m_currentShip = ( m_currentShip - 1 ) % size;
	(*(m_shipNode->getChildren().begin()+m_currentShip))->setVisible(true);
	
}
