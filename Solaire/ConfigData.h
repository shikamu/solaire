#pragma once

#include <irrTypes.h>
#include <EDriverTypes.h>
#include "EventReceiver.h"
#include "KeyBindings.h"

class DeviceCreationParameters;

namespace irr
{
	class IrrlichtDevice;
}

class ConfigData
{
public:

	ConfigData();
	~ConfigData();

	irr::IrrlichtDevice* createDevice();

	irr::IrrlichtDevice* getDevice() const;

	void setDriverType(const irr::video::E_DRIVER_TYPE type);
	void setWindowWidth(const irr::u32 width);
	void setWindowHeight(const irr::u32 height);
	void setFullscreen(const bool f);
	void setBits(const irr::u8 bits);
	void setZBufferBits(const irr::u8 bits);
	void setVSync(const bool v);
	void setAntiAlias(const irr::u8 aa);
	void setGamma(const irr::f32 gamma);

	irr::video::E_DRIVER_TYPE getDriverType() const;
	irr::u32 getWindowWidth() const;
	irr::u32 getWindowHeight() const;
	bool getFullscreen() const;
	irr::u8 getBits() const;
	bool getVSync() const;
	irr::u8 getAntiAlias() const;
	irr::f32 getGamma() const;

	EventReceiver& getEventReceiver();

	KeyBindings& getKeybindings();

	void registerKeybindings();
	void unregisterKeybindings();

private:
	EventReceiver m_eventReceiver;
	KeyBindings m_keybindings;
	irr::IrrlichtDevice* m_device;
	DeviceCreationParameters* m_params;

	bool m_keybindingsRegistered;
};

