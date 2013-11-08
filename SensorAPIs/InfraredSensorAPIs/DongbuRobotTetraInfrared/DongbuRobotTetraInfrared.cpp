
#include "dsphal.h"
#include "DongbuRobotTetraInfrared.h"
#include "OprosPrintMessage.h"

#define PSD_MAX_SIZE			16


DongbuRobotTetraInfrared::DongbuRobotTetraInfrared(void)
{

}

DongbuRobotTetraInfrared::~DongbuRobotTetraInfrared(void)
{

}

int DongbuRobotTetraInfrared::Initialize(Property parameter)
{
	if(SetParameter(parameter) == API_ERROR) {
		PrintMessage("DongbuRobotTetraInfrared::Initialize() ERROR -> SetParameter()\n");
		return API_ERROR;
	}

	return API_SUCCESS;
}

int DongbuRobotTetraInfrared::Finalize(void)
{
	return API_SUCCESS;
}

int DongbuRobotTetraInfrared::Enable(void)
{
	return API_SUCCESS;
}

int DongbuRobotTetraInfrared::Disable(void)
{
	return API_SUCCESS;
}

int DongbuRobotTetraInfrared::SetParameter(Property parameter)
{
	if (parameter.FindName("Size") == false) return API_ERROR;
	if (parameter.FindName("RobotIP") == false) return API_ERROR;
	if (parameter.FindName("RobotPort") == false) return API_ERROR;
	
	sensorCount = atoi (parameter.GetValue("Size").c_str());
	robotIP = parameter.GetValue("RobotIP");
	robotPort = atoi (parameter.GetValue("RobotPort").c_str());

	PrintMessage("-- DongbuRobotTetraInfrared Parameter\n");
	PrintMessage("-- Size              : %d\n", sensorCount);
	PrintMessage("-- RobotIP           : %s\n", robotIP.c_str ());
	PrintMessage("-- RobotPort         : %d\n", robotPort);
	PrintMessage("\n");

	this->parameter = parameter;

	return API_SUCCESS;
}

int DongbuRobotTetraInfrared::GetParameter(Property &parameter)
{
	parameter = this->parameter;

	return API_SUCCESS;
}

int DongbuRobotTetraInfrared::GetSensorValue(vector<double> &sensorValue)
{
	vector<int> tmp;
	tmp.resize(PSD_MAX_SIZE);
	sensorValue.clear();
	int size;

	if (!ReadPsdSensorArray (&tmp[0], size)) {
		return API_ERROR;
	}

	for(size_t i = 0; i < tmp.size(); i++) {
		sensorValue.push_back((double)tmp[i] * 0.01);
	}

	return API_SUCCESS;
}

bool DongbuRobotTetraInfrared::ReadPsdSensorArray (int sensorValue [], int &sensorSize)
{
	bool ret = false;
	dsphal_tcp_client_t *tcp_client;

	if ((tcp_client = dsphal_tcp_client_create((char *)robotIP.c_str (), robotPort)) == NULL) {
		PrintMessage ("ERROR : can't create client to Tetra\n");
		return false;
	}

	if (0 <= dsphal_tcp_client_connect(tcp_client)) {
		dsphal_datalist_t *datalist_ret = dsphal_request_method_call(tcp_client, (char *)"ReadPsdSensorArray", NULL);
		if (datalist_ret) {
			int psd_sensor_val[PSD_MAX_SIZE];
			dsphal_decompose_root_datalist(datalist_ret, (char *)"[{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}{i}]",
				&psd_sensor_val[0],
				&psd_sensor_val[1],
				&psd_sensor_val[2],
				&psd_sensor_val[3],
				&psd_sensor_val[4],
				&psd_sensor_val[5],
				&psd_sensor_val[6],
				&psd_sensor_val[7],
				&psd_sensor_val[8],
				&psd_sensor_val[9],
				&psd_sensor_val[10],
				&psd_sensor_val[11],
				&psd_sensor_val[12],
				&psd_sensor_val[13],
				&psd_sensor_val[14],
				&psd_sensor_val[15]);

			if (sensorValue) {
				int count = 0;
				for (int i = 0; i < sensorSize && i < PSD_MAX_SIZE; i++) {
					sensorValue[i] = psd_sensor_val[i];
					++count;
				}
				sensorSize = count;
			}
			else {
				sensorSize = 0;
			}

			dsphal_datalist_destroy(datalist_ret);
			ret = true;
		}
		else {
			PrintMessage ("ERROR : datalist_ret is NULL\n");
			ret = false;
		}
	}
	else {
		PrintMessage ("ERROR : can't connect to Tetra\n");
		ret = false;
	}

	dsphal_tcp_client_destroy(tcp_client);

	return ret;
}

#if !defined(DEVICE_TEST)
#if defined(WIN32)
extern "C"
{
	__declspec(dllexport) OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new DongbuRobotTetraInfrared();
}
#else
extern "C"
{
	OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new DongbuRobotTetraInfrared();
}
#endif
#endif