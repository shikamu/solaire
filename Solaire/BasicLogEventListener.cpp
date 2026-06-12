#include <fstream>

#include "BasicLogEventListener.h"


BasicLogEventListener::BasicLogEventListener()
{
}


BasicLogEventListener::~BasicLogEventListener()
{
}

bool BasicLogEventListener::onLogEvent(const irr::SEvent::SLogEvent& evt)
{
	std::ofstream file;
	file.open("Output.txt", std::ios::app);
	file << evt.Text << std::endl;
	file.close();
	return false;
}