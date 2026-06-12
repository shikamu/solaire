#include <irrlicht.h>

#include "OptionsGUIView.h"
#include "System.h"
#include "ConfigData.h"
#include "GUIViewAnimator.h"
#include "GUIConstants.h"
#include "KeybindingListener.h"
#include "Utilities.h"
#include "InputConstants.h"

using namespace irr;



OptionsGUIView::OptionsGUIView(MenuScene* parent) : GUIView(parent), m_inputListener(NULL), m_animator(NULL), 
	m_text1(NULL), m_videoDriver(NULL), m_text2(NULL), m_videoMode(NULL), m_fullscreen(NULL), m_text3(NULL), m_bit32(NULL), 
	m_text4(NULL), m_vsync(NULL), m_text5(NULL), m_multiSample(NULL), m_text6(NULL), m_gamma(NULL), m_text7(NULL), 
	m_apply(NULL), m_tabs(NULL), m_keybindingTableLocked(false)
{

	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();
	core::dimension2d<u32> dim = driver->getScreenSize();

	gui::EGUI_DEFAULT_COLOR b = gui::EGDC_3D_HIGH_LIGHT;
	video::SColor col = env->getSkin()->getColor(b);
	col.setAlpha(255);
	env->getSkin()->setColor(b, col);

	b = gui::EGDC_SCROLLBAR;
	col = env->getSkin()->getColor(b);
	col.setAlpha(255);
	env->getSkin()->setColor(b, col);


	m_inputListener = new KeybindingListener(this);

	m_tabs = env->addTabControl(core::rect<s32>(static_cast<s32>(dim.Width*0.3f), static_cast<s32>(dim.Height*0.35f), static_cast<s32>(dim.Width*0.95f), static_cast<s32>(dim.Height*0.9f)), NULL, false, false, GUI_ID_OPTIONSMENU_VIDEODRIVER_TAB);

	gui::IGUITab* video = m_tabs->addTab(L"video");
	gui::IGUITab* sound = m_tabs->addTab(L"sound");
	gui::IGUITab* controls = m_tabs->addTab(L"controls");
	
	createVideoTab(video);
	createControlsTab(controls);
	

	hide();
}

void OptionsGUIView::createVideoTab(irr::gui::IGUITab* video)
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();

	core::dimension2d<u32> dim(video->getAbsoluteClippingRect().getSize());

	float minXEver = 0.05f;

	float textX = minXEver;
	float textY = 0.1f;
	//float textW = 0.15f;
	//float textH = 0.045f;
	s32 textW = 125;
	s32 textH = 30;
	//float compX = textX+textW;
	float compX = textX+static_cast<f32>(textW)/static_cast<f32>(dim.Width);
	float compY = textY;
	//float compW = 0.35f;
	s32 compW = 300;
	//float compH = textH;
	s32 compH = textH;
	//const float ySpace = 0.025f;
	const f32 ySpace = 0.15f;
	const f32 xSpace = 0.025f;
	f32 temp1, temp2;
	s32 temp3, temp4;


	m_text1 = env->addStaticText(L"VideoDriver:", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text1->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_videoDriver = env->addComboBox(core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX)+compW), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_VIDEODRIVER_COMBO);
	m_elements.push_back(m_text1);
	m_elements.push_back(m_videoDriver);
	
	const float maxXever = dim.Width*(compX)+compW;

	m_videoDriver->addItem(L"Direct3D 9.0c", video::EDT_DIRECT3D9);
	m_videoDriver->addItem(L"Direct3D 8.1", video::EDT_DIRECT3D8);
	m_videoDriver->addItem(L"OpenGL 1.5", video::EDT_OPENGL);
	m_videoDriver->addItem(L"Software Renderer", video::EDT_SOFTWARE);
	m_videoDriver->addItem(L"Burning's Video (TM)", video::EDT_BURNINGSVIDEO);
	m_videoDriver->setSelected(m_videoDriver->getIndexForItemData(System::get().getConfig()->getDriverType()));
	m_videoDriver->setToolTipText (L"Use a VideoDriver");

	m_animationPosition1X = dim.Width*textX;
	m_animationPosition1Y = dim.Height*textY;

	m_animationPosition2X = dim.Width*compX;
	m_animationPosition2Y = dim.Height*compY;
	
	//textY += textH+ySpace;
	textY += ySpace;
	compY = textY;

	m_text2 = env->addStaticText (L"VideoMode:", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text2->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_videoMode = env->addComboBox(core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX)+compW), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_VIDEOMODE_COMBO);
	m_videoMode->setToolTipText(L"Supported Screenmodes");
	m_elements.push_back(m_text2);
	m_elements.push_back(m_videoMode);

	video::IVideoModeList *modeList = System::get().getDevice()->getVideoModeList();
	if (modeList)
	{
		char buff[256];
		for (s32 i = 0; i != modeList->getVideoModeCount(); ++i)
		{
			u16 d = modeList->getVideoModeDepth(i);
			if (d < 16)
				continue;

			u16 w = modeList->getVideoModeResolution(i).Width;
			u16 h = modeList->getVideoModeResolution(i).Height;
			u32 val = w << 16 | h;

			if (m_videoMode->getIndexForItemData(val) >= 0)
				continue;

			f32 aspect = (f32) w / (f32) h;
			const c8 *a = "";
			if (core::equals(aspect, 1.3333333333f)) a = "4:3";
			else if(core::equals(aspect, 1.6666666f)) a = "15:9 widescreen";
			else if(core::equals(aspect, 1.7777777f)) a = "16:9 widescreen";
			else if(core::equals(aspect, 1.6f)) a = "16:10 widescreen";
			else if(core::equals(aspect, 2.133333f)) a = "20:9 widescreen";
			if(strcmp("", a)==0)
				snprintf(buff, sizeof(buff), "%d x %d", w, h);
			else
				snprintf(buff, sizeof(buff), "%d x %d, %s", w, h, a);
			m_videoMode->addItem(core::stringw(buff).c_str(), val);
		}
	}
	m_videoMode->setSelected(m_videoMode->getIndexForItemData(System::get().getConfig()->getWindowWidth() << 16 | System::get().getConfig()->getWindowHeight()));

	m_animationPosition3X = dim.Width*textX;
	m_animationPosition3Y = dim.Height*textY;

	m_animationPosition4X = dim.Width*compX;
	m_animationPosition4Y = dim.Height*compY;

						
	//textY += textH+ySpace;
	textY += ySpace;
	temp1 = textX;
	temp2 = compX;
	temp3 = compW;
	temp4 = textW;
	compX = textX;
	compY = textY;
	//compW = 0.03f;
	compW = compH;
	//textX = compX + compW;
	textX = compX + static_cast<f32>(compW)/static_cast<f32>(dim.Width);
	//textW = 0.125f;
	textW = 100;

	m_fullscreen = env->addCheckBox(System::get().getConfig()->getFullscreen(), core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX)+compW), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_FULLSCREEN_CHECKBOX);
	m_fullscreen->setToolTipText(L"Set Fullscreen or Window Mode" );
	m_text3 = env->addStaticText (L"Fullscreen", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text3->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_elements.push_back(m_text3);
	m_elements.push_back(m_fullscreen);


	m_animationPosition5X = dim.Width*textX;
	m_animationPosition5Y = dim.Height*textY;

	m_animationPosition6X = dim.Width*compX;
	m_animationPosition6Y = dim.Height*compY;


	compX = textX + static_cast<f32>(textW)/static_cast<f32>(dim.Width) + xSpace;
	textX = compX + static_cast<f32>(compW)/static_cast<f32>(dim.Width);
	m_bit32 = env->addCheckBox(System::get().getConfig()->getBits() == 32, core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX+compW)), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_32BITS_CHECKBOX);
	m_bit32->setToolTipText(L"Use 16 or 32 Bit");
	m_text4 = env->addStaticText (L"32Bits", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text4->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_elements.push_back(m_text4);
	m_elements.push_back(m_bit32);



	m_animationPosition7X = dim.Width*textX;
	m_animationPosition7Y = dim.Height*textY;

	m_animationPosition8X = dim.Width*compX;
	m_animationPosition8Y = dim.Height*compY;


	compX = textX + static_cast<f32>(textW)/static_cast<f32>(dim.Width) + xSpace;
	textX = compX + static_cast<f32>(compW)/static_cast<f32>(dim.Width);
	m_vsync = env->addCheckBox(System::get().getConfig()->getVSync(), core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX+compW)), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_VSYNC_CHECKBOX);
	m_vsync->setToolTipText(L"Use Vertical Sync");
	m_text5 = env->addStaticText (L"VSync", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text5->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_elements.push_back(m_text5);
	m_elements.push_back(m_vsync);


	m_animationPosition9X = dim.Width*textX;
	m_animationPosition9Y = dim.Height*textY;

	m_animationPosition10X = dim.Width*compX;
	m_animationPosition10Y = dim.Height*compY;


	textX = temp1;
	compX = temp2;	
	compW = static_cast<s32>(temp3*0.85f);
	textW = temp4;
	//textY += textH+ySpace;
	textY += ySpace;
	compY = textY;
						
	m_text6 = env->addStaticText(L"MultiSample:", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text6->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_multiSample = env->addScrollBar(true, core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX)+compW), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_MULTISAMPLE_BAR);
	m_multiSample->setMin(0);
	m_multiSample->setMax(8);
	m_multiSample->setSmallStep(1);
	m_multiSample->setLargeStep(1);
	m_multiSample->setPos(System::get().getConfig()->getAntiAlias());
	m_multiSample->setToolTipText(L"Set the MultiSample (disable, 1x, 2x, 4x, 8x )");
	m_elements.push_back(m_text6);
	m_elements.push_back(m_multiSample);



	m_animationPosition11X = dim.Width*textX;
	m_animationPosition11Y = dim.Height*textY;

	m_animationPosition12X = dim.Width*compX;
	m_animationPosition12Y = dim.Height*compY;


	textY += ySpace;
	compY = textY;
	m_text7 = env->addStaticText(L"Gamma:", core::rect<s32>(static_cast<s32>(dim.Width*textX), static_cast<s32>(dim.Height*textY), static_cast<s32>(dim.Width*(textX)+textW), static_cast<s32>(dim.Height*(textY)+textH)), false, false);
	m_text7->setOverrideColor(video::SColor(255, 255, 255, 255));
	m_gamma = env->addScrollBar(true, core::rect<s32>(static_cast<s32>(dim.Width*compX), static_cast<s32>(dim.Height*compY), static_cast<s32>(dim.Width*(compX)+compW), static_cast<s32>(dim.Height*(compY)+compH)), NULL, GUI_ID_OPTIONSMENU_GAMMA_BAR);
	m_gamma->setMin(50);
	m_gamma->setMax(350);
	m_gamma->setSmallStep(1);
	m_gamma->setLargeStep(10);
	float gamma = System::get().getConfig()->getGamma();
	m_gamma->setPos(core::floor32(gamma * 100.f));
	m_gamma->setToolTipText(L"Adjust Gamma Ramp ( 0.5 - 3.5)");
	m_elements.push_back(m_text7);
	m_elements.push_back(m_gamma);

	System::get().getConfig()->setGamma(gamma);

	m_animationPosition13X = dim.Width*textX;
	m_animationPosition13Y = dim.Height*textY;

	m_animationPosition14X = dim.Width*compX;
	m_animationPosition14Y = dim.Height*compY;


	float deltaY = 0.05f;
	u32 buttonHeight = static_cast<u32>(dim.Height - (m_gamma->getAbsolutePosition().LowerRightCorner.Y + deltaY*dim.Height));
	u32 buttonWidth = 2*buttonHeight;
	
	float x = textX;
	
	m_apply = env->addButton(core::rect<s32>(static_cast<s32>(dim.Width*x+(maxXever - dim.Width*x - buttonWidth)*0.5f), static_cast<s32>(m_gamma->getAbsolutePosition().LowerRightCorner.Y+ deltaY*dim.Height), static_cast<s32>(dim.Width*x+(maxXever - dim.Width*x - buttonWidth)*0.5f + buttonWidth), static_cast<s32>(dim.Height)), NULL, GUI_ID_OPTIONSMENU_APPLY_BUTTON, L"APPLY", L"Apply settings with current values");
	m_apply->setImage(driver->getTexture("TestButtonUp.tga"));
	m_apply->setPressedImage(driver->getTexture("TestButtonDown.tga"));
	m_apply->setScaleImage(true);
	m_apply->setUseAlphaChannel(true);
	m_apply->setDrawBorder(false);
	//m_apply->setOverrideFont(env->getFont("agency14.xml"));
	m_elements.push_back(m_apply);

	m_animationPosition15X = dim.Width*x+(maxXever - dim.Width*x - buttonWidth)*0.5f;
	m_animationPosition15Y = m_gamma->getAbsolutePosition().LowerRightCorner.Y + deltaY*dim.Height;


	for(std::vector<gui::IGUIElement*>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		video->addChild(*it);
	}

}

void OptionsGUIView::createControlsTab(irr::gui::IGUITab* controls)
{
	gui::IGUIEnvironment* env = System::get().getDevice()->getGUIEnvironment();
	video::IVideoDriver* driver = System::get().getDevice()->getVideoDriver();

	core::dimension2d<u32> dim(controls->getAbsoluteClippingRect().getSize());

	gui::IGUITable* table = env->addTable(core::rect<s32>(0, 5, dim.Width, dim.Height), controls, GUI_ID_OPTIONSMENU_KEYBINDINGS_TABLE);
	table->addColumn(L"Action", 0);
	table->addColumn(L"Primary Bind", 1);
	table->addColumn(L"Secondary Bind", 2);
	table->setColumnWidth(0, static_cast<u32>(table->getAbsoluteClippingRect().getWidth()*0.33f));
	table->setColumnWidth(1, static_cast<u32>(table->getAbsoluteClippingRect().getWidth()*0.33f));
	table->setColumnWidth(2, static_cast<u32>(table->getAbsoluteClippingRect().getWidth()*0.33f));
	table->setColumnOrdering(0, gui::EGCO_NONE);
	table->setColumnOrdering(1, gui::EGCO_NONE);
	table->setColumnOrdering(2, gui::EGCO_NONE);
	table->setDrawFlags(gui::EGTDF_ACTIVE_ROW);
	table->setResizableColumns(false);
	
	unsigned int row = 0;
	for(unsigned int action = NOT_AN_ACTION_FIRST + 1; action < NOT_AN_ACTION_COUNT; ++action)
	{
		table->addRow(row);
		table->setCellText(row, 0, Utilities::getActionName((Action)action), video::SColor(255, 255, 255, 255));
		table->setCellText(row, 1, System::get().getConfig()->getKeybindings().getBindingFor((Action)action, true), video::SColor(255, 255, 255, 255));
		table->setCellText(row, 2, System::get().getConfig()->getKeybindings().getBindingFor((Action)action, false), video::SColor(255, 255, 255, 255));
		Binding* b1 = System::get().getConfig()->getKeybindings().getBinding((Action)action, true);
		Binding* b2 = System::get().getConfig()->getKeybindings().getBinding((Action)action, false);
		if(!b1)
			b1 = System::get().getConfig()->getKeybindings().rebindEmpty((Action)action, BINDING_NOTYPE, true);
		if(!b2)
			b2 = System::get().getConfig()->getKeybindings().rebindEmpty((Action)action, BINDING_NOTYPE, false);

		table->setCellData(row, 1, b1);
		table->setCellData(row, 2, b2);
		++row;
	}
	
	//reserved for gui rendering reasons (if there's an horizontal scrollbar, e.g. with low resolution, then the last element will be harder to see therefore we add a dummy one)
 	table->addRow(row);
	table->setCellText(row, 0, L"", video::SColor(255, 255, 255, 255));
	table->setCellText(row, 1, L"", video::SColor(255, 255, 255, 255));
	table->setCellText(row, 2, L"", video::SColor(255, 255, 255, 255));	

	m_elements.push_back(table);
	controls->addChild(table);
	
}

void OptionsGUIView::registerInputListener()
{
	System::get().registerEventListener(m_inputListener);
}

void OptionsGUIView::unregisterInputListener()
{
	System::get().unregisterEventListener(m_inputListener);
}

OptionsGUIView::~OptionsGUIView()
{
	hide();
	if(m_inputListener)
	{
		unregisterInputListener();
		delete m_inputListener;
		m_inputListener = NULL;
	}
}

void OptionsGUIView::show()
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

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition1X), static_cast<s32>(m_animationPosition1Y), m_text1));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition2X), static_cast<s32>(m_animationPosition2Y), m_videoDriver));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition3X), static_cast<s32>(m_animationPosition3Y), m_text2));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition4X), static_cast<s32>(m_animationPosition4Y), m_videoMode));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition5X), static_cast<s32>(m_animationPosition5Y), m_text3));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition6X), static_cast<s32>(m_animationPosition6Y), m_fullscreen));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition7X), static_cast<s32>(m_animationPosition7Y), m_text4));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition8X), static_cast<s32>(m_animationPosition8Y), m_bit32));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition9X), static_cast<s32>(m_animationPosition9Y), m_text5));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition10X), static_cast<s32>(m_animationPosition10Y), m_vsync));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition11X), static_cast<s32>(m_animationPosition11Y), m_text6));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition12X), static_cast<s32>(m_animationPosition12Y), m_multiSample));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition13X), static_cast<s32>(m_animationPosition13Y), m_text7));
	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition14X), static_cast<s32>(m_animationPosition14Y), m_gamma));

	m_animator->addElement(new GUIAnimation(static_cast<s32>(m_animationPosition15X), static_cast<s32>(m_animationPosition15Y), m_apply));


	m_tabs->setVisible(true);
	GUIView::show();
	m_animator->start();
}

void OptionsGUIView::hide()
{
	if(m_animator)
	{
		//m_animator->interrupt();
		m_animator->finish();
		delete m_animator;
		m_animator = NULL;
	}
	if(m_inputListener)
	{
		System::get().unregisterEventListener(m_inputListener);
	}
	m_keybindingTableLocked = false;

	m_tabs->setVisible(false);
	GUIView::hide();
}

void OptionsGUIView::setKeybindingTableLocked(const bool b)
{
	m_keybindingTableLocked = b;
	if(!m_keybindingTableLocked)
		System::get().getConfig()->getKeybindings().saveBindings();
}



