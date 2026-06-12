#include "GUIView.h"

#include <IGUIElement.h>

using namespace irr;

GUIView::GUIView(MenuScene* parent) : m_parent(parent)
{
}


GUIView::~GUIView()
{
	for(std::vector<irr::gui::IGUIElement*>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		(*it)->remove();
	}
}

void GUIView::show()
{
	for(std::vector<gui::IGUIElement*>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		(*it)->setVisible(true);
	}
}

void GUIView::hide()
{
	for(std::vector<gui::IGUIElement*>::const_iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		(*it)->setVisible(false);
	}
}
