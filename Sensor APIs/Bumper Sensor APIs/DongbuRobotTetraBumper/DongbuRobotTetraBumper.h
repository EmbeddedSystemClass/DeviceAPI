#ifndef _DASAROBOT_TETRABUMPER_H_
#define _DASAROBOT_TETRABUMPER_H_

#include "BumperSensor.h"

class DongbuRobotTetraBumper : public BumperSensor
{
public:
	DongbuRobotTetraBumper(void);
	virtual ~DongbuRobotTetraBumper(void);

public:
	int Initialize(Property parameter);
	int Finalize(void);
	int Enable(void);
	int Disable(void);
	int SetParameter(Property parameter);
	int GetParameter(Property &parameter);

	int GetSensorValue(vector<bool> &sensorValue);

private:
	int bumperCount;
	std::string robotIP;
	int robotPort;

	bool ReadBumperArray (bool *bumperValue, int &bumperSize);
	void SetBumperDirMode ();
	void SetBumperOffMode ();

#if defined(WIN32)
	HMODULE hOprosAPI;
#endif

};

#endif