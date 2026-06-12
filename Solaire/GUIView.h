#pragma once
#include <vector>

class MenuScene;

namespace irr
{
	namespace gui
	{
		class IGUIElement;
	}
}


class GUIView
{
public:
	GUIView(MenuScene* parent);
	virtual ~GUIView();
	virtual void show();
	virtual void hide();

	virtual GUIView& operator=(const GUIView&){
		return *this;
	}


protected:
	std::vector<irr::gui::IGUIElement*> m_elements;

	//we need this if we want to add 3d content to a gui view, that way we can get to the scenemanager
	MenuScene* m_parent;
};

