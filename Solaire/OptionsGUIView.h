#pragma once
#include "GUIView.h"

class GUIViewAnimator;
class KeybindingListener;

namespace irr
{
	namespace gui
	{
		class IGUIStaticText;
		class IGUIComboBox;
		class IGUICheckBox;
		class IGUIScrollBar;
		class IGUIButton;
		class IGUITabControl;
		class IGUITab;
	}
}

class OptionsGUIView : public GUIView
{

friend class MenuScene;

public:
	OptionsGUIView(MenuScene* parent);
	~OptionsGUIView();
	
	void show();
	void hide();

	bool isKeybindingTableLocked() const
	{
		return m_keybindingTableLocked;
	}

	void setKeybindingTableLocked(const bool b);

private:

	void createVideoTab(irr::gui::IGUITab* tab);
	void createControlsTab(irr::gui::IGUITab* tab);
	void registerInputListener();
	void unregisterInputListener();

	KeybindingListener* m_inputListener;

	GUIViewAnimator* m_animator;

	irr::gui::IGUIStaticText* m_text1;
	irr::gui::IGUIComboBox* m_videoDriver;

	irr::gui::IGUIStaticText* m_text2;
	irr::gui::IGUIComboBox* m_videoMode;

	irr::gui::IGUICheckBox* m_fullscreen;
	irr::gui::IGUIStaticText* m_text3;

	irr::gui::IGUICheckBox* m_bit32;
	irr::gui::IGUIStaticText* m_text4;

	irr::gui::IGUICheckBox* m_vsync;
	irr::gui::IGUIStaticText* m_text5;

	irr::gui::IGUIScrollBar* m_multiSample;
	irr::gui::IGUIStaticText* m_text6;

	irr::gui::IGUIScrollBar* m_gamma;
	irr::gui::IGUIStaticText* m_text7;

	irr::gui::IGUIButton* m_apply;

	irr::gui::IGUITabControl* m_tabs;

	float m_animationPosition1X, m_animationPosition1Y, m_animationPosition2X, m_animationPosition2Y, m_animationPosition3X, m_animationPosition3Y, 
		m_animationPosition4X, m_animationPosition4Y, m_animationPosition5X, m_animationPosition5Y, m_animationPosition6X, m_animationPosition6Y, 
		m_animationPosition7X, m_animationPosition7Y, m_animationPosition8X, m_animationPosition8Y, m_animationPosition9X, m_animationPosition9Y,
		m_animationPosition10X, m_animationPosition10Y, m_animationPosition11X, m_animationPosition11Y, m_animationPosition12X, m_animationPosition12Y,
		m_animationPosition13X, m_animationPosition13Y, m_animationPosition14X, m_animationPosition14Y, m_animationPosition15X, m_animationPosition15Y;

	bool m_keybindingTableLocked;

};

