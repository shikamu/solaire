#pragma once
#include "GUIView.h"

class GUIViewAnimator;
class StandardShader;

namespace irr
{
	namespace gui
	{
		class IGUIButton;
	}
	namespace scene
	{
		class ISceneNode;
		class ICameraSceneNode;
	}
}


class SinglePlayerGUIView : public GUIView
{
public:
	SinglePlayerGUIView(MenuScene* parent);
	~SinglePlayerGUIView();

	void show();
	void hide();

	void toggleNextShip();
	void togglePreviousShip();
	

private:
	
	GUIViewAnimator* m_animator;
	irr::gui::IGUIButton* m_leftChevron;
	irr::gui::IGUIButton* m_rightChevron;
	irr::gui::IGUIButton* m_play;

	irr::scene::ICameraSceneNode* m_camera;
	irr::scene::ISceneNode* m_shipNode;
	
	StandardShader* m_shipShader1, *m_shipShader2;

	unsigned int m_currentShip;

	float m_chevronWidth;
	float m_chevronHeight;
	float m_leftChevronX;
	float m_leftChevronY;
	float m_rightChevronX;
	float m_rightChevronY;
	float m_playWidth;
};

