#pragma once
#include <irrTypes.h>
#include <SIrrCreationParameters.h>


class DeviceCreationParameters
{
public:
	DeviceCreationParameters();
	~DeviceCreationParameters();

	irr::SIrrlichtCreationParameters params;

	irr::f32 gammaValue;

	void setDefaults();

};

