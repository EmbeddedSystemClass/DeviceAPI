#pragma once

#include "UltrasonicSensor.h"
#include "I2C.h"

class DevantechSrf08 : public UltrasonicSensor
{
public:
	DevantechSrf08(void);
	virtual ~DevantechSrf08(void);

	int Initialize(Property parameter);
	int Finalize(void);
	int Enable(void);
	int Disable(void);
	int SetParameter(Property parameter);
	int GetParameter(Property &parameter);

	int GetSensorValue(vector<double> &sensorValue);

private:
	I2C *_i2c;

#if defined(WIN32)
	HMODULE hOprosAPI;
#else
	void *hOprosAPI;
#endif

	int InitializeI2C(Property parameter);
	int FinalizeI2C(void);
};

