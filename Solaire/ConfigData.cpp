#include <irrlicht.h>

#include "ConfigData.h"
#include "DeviceCreationParameters.h"

using namespace irr;


ConfigData::ConfigData() : m_params(new DeviceCreationParameters), m_device(NULL), m_keybindingsRegistered(false)
{
	m_params->setDefaults();
}

ConfigData::~ConfigData()
{
	delete m_params;
	m_params = NULL;
}

irr::IrrlichtDevice* ConfigData::createDevice()
{
	m_device = createDeviceEx(m_params->params);
	m_device->getFileSystem()->addFolderFileArchive("data/");
	m_eventReceiver.reinit();
	m_device->setEventReceiver(&m_eventReceiver);
	return m_device;
}

irr::IrrlichtDevice* ConfigData::getDevice() const
{
	return m_device;
}

void ConfigData::setDriverType(const irr::video::E_DRIVER_TYPE type)
{
	m_params->params.DriverType = type;
}

void ConfigData::setWindowWidth(const u32 width)
{
	m_params->params.WindowSize.Width = width;
}

void ConfigData::setWindowHeight(const u32 height)
{
	m_params->params.WindowSize.Height = height;
}

void ConfigData::setFullscreen(const bool f)
{
	m_params->params.Fullscreen = f;
}

void ConfigData::setBits(const u8 bits)
{
	m_params->params.Bits = bits;
}

void ConfigData::setZBufferBits(const u8 bits)
{
	m_params->params.ZBufferBits = bits;
}

void ConfigData::setVSync(const bool v)
{
	m_params->params.Vsync = v;
}

void ConfigData::setAntiAlias(const u8 aa)
{
	m_params->params.AntiAlias = aa;
}

void ConfigData::setGamma(const irr::f32 gamma)
{
	m_params->gammaValue = gamma;
	if(m_device)
		m_device->setGammaRamp(gamma, gamma, gamma, 0.0f, 0.0f);
}

video::E_DRIVER_TYPE ConfigData::getDriverType() const
{
	return m_params->params.DriverType;
}

u32 ConfigData::getWindowWidth() const
{
	return m_params->params.WindowSize.Width;
}

u32 ConfigData::getWindowHeight() const
{
	return m_params->params.WindowSize.Height;
}

bool ConfigData::getFullscreen() const
{
	return m_params->params.Fullscreen;
}

u8 ConfigData::getBits() const
{
	return m_params->params.Bits;
}

bool ConfigData::getVSync() const
{
	return m_params->params.Vsync;
}

u8 ConfigData::getAntiAlias() const
{
	return m_params->params.AntiAlias;
}

f32 ConfigData::getGamma() const
{
	return m_params->gammaValue;
}

EventReceiver& ConfigData::getEventReceiver()
{
	return m_eventReceiver;
}

KeyBindings& ConfigData::getKeybindings()
{
	return m_keybindings;
}

void ConfigData::registerKeybindings()
{
	if(!m_keybindingsRegistered)
	{
		m_eventReceiver.registerEventListener(&m_keybindings);
		m_keybindingsRegistered = true;
	}
}

void ConfigData::unregisterKeybindings()
{
	m_eventReceiver.unregisterEventListener(&m_keybindings);
	m_keybindingsRegistered = false;
}

