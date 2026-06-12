#include <irrlicht.h>

#include "DeviceCreationParameters.h"

using namespace irr;

DeviceCreationParameters::DeviceCreationParameters() : gammaValue(1.0f)
{
}


DeviceCreationParameters::~DeviceCreationParameters()
{
}

void DeviceCreationParameters::setDefaults()
{

	gammaValue = 1.0f;

#ifdef _IRR_WINDOWS_
	params.DriverType = video::EDT_DIRECT3D9;
#else
	params.DriverType = video::EDT_OPENGL;
#endif

	//params.WindowSize.Width = 800;
    //params.WindowSize.Height = 600;
	params.WindowSize.Width = 1280;
    params.WindowSize.Height = 720;
	//params.WindowSize.Width = 1920;
    //params.WindowSize.Height = 1080;
    params.Fullscreen = false;
    params.Bits = 24;
    params.ZBufferBits = 16;
    params.Vsync = true;
    params.AntiAlias = false;
}
