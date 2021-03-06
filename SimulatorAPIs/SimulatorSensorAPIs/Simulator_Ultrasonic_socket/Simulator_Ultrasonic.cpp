#include "data.h"
#include "Simulator_Ultrasonic.h"
#include "OprosPrintMessage.h"
#include <string>

using namespace std;

string ParsingUserMessage(string *userMsg)
{
	string tmp = "";
	int len = userMsg->find(";");

	tmp.assign(*userMsg, 0, len);
	userMsg->erase(0, len + 1);

	return tmp;
}

Simulator_Ultrasonic::Simulator_Ultrasonic(void)
{
}

Simulator_Ultrasonic::~Simulator_Ultrasonic(void)
{
}

int Simulator_Ultrasonic::Initialize(Property parameter)
{
	if(SetParameter(parameter) != API_SUCCESS)	{
		PrintMessage("Simulator_UltrasonicSensorComp :: initialize fails\n");
		return API_ERROR;
	}
	PrintMessage("Simulator_UltrasonicSensorComp :: initalize Success\n");
	return API_SUCCESS;

	if (SetParameter (parameter) == false) {
		return false;
	}
}

int Simulator_Ultrasonic::Enable(void)
{
	if (m_ClientSocket.enable(SimulatorIP,PortNumber) == false)	{
		return API_ERROR;
	}
	redata = m_ClientSocket.findSystem(RobotName,DeviceName,sensorCount);

	return API_SUCCESS;
}

int Simulator_Ultrasonic::Disable(void)
{
	return API_SUCCESS;
}

int Simulator_Ultrasonic::Finalize(void)
{
	return API_SUCCESS;
}

int Simulator_Ultrasonic::SetParameter(Property parameter)
{

	SimulatorIP = parameter.GetValue("IP").c_str();
	sensorCount = atoi(parameter.GetValue("Size").c_str());
	deviceName = parameter.GetValue("deviceName").c_str();
	RobotName = parameter.GetValue("RobotName").c_str();
	PortNumber	= atoi(parameter.GetValue("Port").c_str());

	if(sensorCount > ULTRA_SONIC_MAX_SIZE)	{
		sensorCount = ULTRA_SONIC_MAX_SIZE;
	}

	for(int i=0 ; i<sensorCount ; i++)	{
		DeviceName[i] = ParsingUserMessage(&deviceName);
	}

	this->parameter = parameter;

	return 0;
}

int Simulator_Ultrasonic::GetParameter(Property &parameter)
{
	parameter = this->parameter;
	return API_SUCCESS;
}

int Simulator_Ultrasonic::GetSensorValue(vector<double> &ultrasonicSensorData)
{
	float *sensorValue = new float [ULTRA_SONIC_MAX_SIZE];

	if(!ReadUltraSonicSensorArray(sensorValue, sensorCount))	{
		delete [] sensorValue;
		return -1;
	}

	for(int i = 0 ; i < sensorCount ; i++)	{
		ultrasonicSensorData.push_back((double)sensorValue[i]);
	}
	delete [] sensorValue;

	return sensorCount;
}


int Simulator_Ultrasonic::ReadUltraSonicSensorArray (float sensorValue [], int &sensorSize)
{

	bool ret = false;
	float value[4];
	int count = 0;

	if(sensorValue){
		for(int i = 0; i < sensorSize && i < ULTRA_SONIC_MAX_SIZE ; i++) {
			if(m_ClientSocket.getValue(redata.deviceID[i],value) == false)
			{
				return false;
			}
			sensorValue[i] = value[0];
			count = count+1;
		}
		sensorSize = count;
		ret = true;
	}else{
		sensorSize = 0;
	}

	return ret;
}

extern "C"
{
__declspec(dllexport) OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new Simulator_Ultrasonic();
}
