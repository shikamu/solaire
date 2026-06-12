#include <irrlicht.h>

#include "GUIViewAnimator.h"
#include "Timer.h"

using namespace irr;


GUIViewAnimator::GUIViewAnimator(const unsigned int duration, IrrlichtDevice* dev) : Task(), m_duration(duration), m_device(dev), m_initialized(true)
{
	
}

GUIViewAnimator::~GUIViewAnimator()
{
	for(std::vector<GUIAnimation*>::iterator it = m_elements.begin(); it != m_elements.end(); ++it)
	{
		delete *it;
	}
	finish();
}

void GUIViewAnimator::addElement(GUIAnimation* element)
{
	m_elements.push_back(element);
}

unsigned int __stdcall GUIViewAnimator::run()
{
	Timer timer;
	timer.start();
	double time = 0.0;
	while(!isInterrupted() && (time = timer.getTimeMilliSec()) < m_duration)
	{
		for(std::vector<GUIAnimation*>::iterator it = m_elements.begin(); !isInterrupted() && it != m_elements.end(); ++it)
		{
			GUIAnimation* anim = *it;			
			anim->element->setRelativePosition(core::position2di(core::lerp(0, anim->dx, static_cast<f32>(time/m_duration)), core::lerp(0, anim->dy, static_cast<f32>(time/m_duration))));
		}
	}

	return 0;
}

