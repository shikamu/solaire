#pragma once

#include <vector>

#include "Task.h"

namespace irr
{
	class IrrlichtDevice;

	namespace gui
	{
		class IGUIElement;
	}
}

struct GUIAnimation
{
	GUIAnimation(const int _dx, const int _dy, irr::gui::IGUIElement* elt) : dx(_dx), dy(_dy), element(elt)
	{}

	const int dx, dy;
	irr::gui::IGUIElement* element;
};

class GUIViewAnimator : public Task{

public:

	void addElement(GUIAnimation* element);
	GUIViewAnimator(const unsigned int duration, irr::IrrlichtDevice* dev);
	~GUIViewAnimator();
	
protected:
	unsigned int __stdcall run();

private:

	std::vector<GUIAnimation*> m_elements;
	irr::IrrlichtDevice* m_device;

	const unsigned int m_duration;			//ms
	bool m_initialized;
};