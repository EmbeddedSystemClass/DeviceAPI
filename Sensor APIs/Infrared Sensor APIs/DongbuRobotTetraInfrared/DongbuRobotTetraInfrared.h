#pragma once

#include "InfraredSensor.h"

class DongbuRobotTetraInfrared : public InfraredSensor
{
public:
	DongbuRobotTetraInfrared(void);
	virtual ~DongbuRobotTetraInfrared(void);

public:
	int Initialize(Property parameter);
	int Finalize(void);
	int Enable(void);
	int Disable(void);
	int SetParameter(Property parameter);
	int GetParameter(Property &parameter);

	int GetSensorValue(vector<double> &sensorValue);

private:
	int sensorCount;
	std::string robotIP;
	int robotPort;

	bool ReadPsdSensorArray (int sensorValue [], int &sensorSize);

#if defined(WIN32)
	HMODULE hOprosAPI;
#endif
};

