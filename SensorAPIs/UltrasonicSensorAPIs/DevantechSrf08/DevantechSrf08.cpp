/*************************************************************

 file: DevantechSrf08.cpp
 author: E.C. Shin
 begin: January 31 2011
 copyright: (c) 2011 KITECH, OPRoS
 email: unchol@kitech.re.kr

***************************************************************

OPRoS source code is provided under a dual license mode:
 LGPL and OPRoS individually.

LGPL: You can redistribute it and/or modify it under the terms
 of the Less GNU General Public License as published by the Free
 Software Foundation, either version 3 of the License.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 Less GNU General Public License for more details. 

 You should have received a copy of the Less GNU General Public
 License along with this program. If not, see 
 <http://www.gnu.org/licenses/>.

OPRoS individual license: An individual license is a license for
 modifying the source code of OPRoS and distiributing it in a
 closed format for commercial purposes.

 If you are interested in this option, please see 
 <http://www.opros.or.kr>.

This license policy may be changed without prior notice.

***************************************************************/

#include "DevantechSrf08.h"
//#include "OprosTimer.h"
#include "OprosPrintMessage.h"


DevantechSrf08::DevantechSrf08(void)
{
	_i2c = NULL;
}


DevantechSrf08::~DevantechSrf08(void)
{
	Finalize();
}

int DevantechSrf08::Initialize(Property parameter)
{
	if(InitializeI2C(parameter) < 0) {
		return API_ERROR;
	}

	return API_SUCCESS;
}

int DevantechSrf08::Finalize(void)
{
	if(_i2c != NULL) {
		delete _i2c;
		_i2c = NULL;
	}

	if(hOprosAPI != NULL) {
#if defined(WIN32)
		FreeLibrary(hOprosAPI);
#else
		dlclose(hOprosAPI);
#endif
		hOprosAPI = NULL;
	}

	return API_SUCCESS;
}

int DevantechSrf08::Enable(void)
{
	return API_SUCCESS;
}

int DevantechSrf08::Disable(void)
{
	return API_SUCCESS;
}

int DevantechSrf08::SetParameter(Property parameter)
{
	if(parameter.FindName("Size") == false)				return API_ERROR;
	int size = atoi(parameter.GetValue("Size").c_str());

	if(size < 0) {
		return API_ERROR;
	}

	char name[128];
	for(int i = 0; i < size; i++) {
		sprintf(name, "X%d", i);
		if(parameter.FindName(string(name)) == false)			goto ERROR_DevantechSrf08_SetParameter;
		
		sprintf(name, "Y%d", i);
		if(parameter.FindName(string(name)) == false)			goto ERROR_DevantechSrf08_SetParameter;
		
		sprintf(name, "Z%d", i);
		if(parameter.FindName(string(name)) == false)			goto ERROR_DevantechSrf08_SetParameter;
		
		sprintf(name, "Roll%d", i);
		if(parameter.FindName(string(name)) == false)			goto ERROR_DevantechSrf08_SetParameter;
		
		sprintf(name, "Pitch%d", i);
		if(parameter.FindName(string(name)) == false)			goto ERROR_DevantechSrf08_SetParameter;
		
		sprintf(name, "Yaw%d", i);
		if(parameter.FindName(string(name)) == false)			goto ERROR_DevantechSrf08_SetParameter;
	}

	this->parameter = parameter;

	return API_SUCCESS;

ERROR_DevantechSrf08_SetParameter :
	PrintMessage("ERROR : DevantechSrf08::SetParameter -> Can't find a property in parameter.\n");
	return API_ERROR;
}

int DevantechSrf08::GetParameter(Property &parameter)
{
	parameter = this->parameter;

	return API_SUCCESS;
}

int DevantechSrf08::GetSensorValue(vector<double> &sensorValue)
{
	if(_i2c == NULL) {
		PrintMessage("ERROR : DevantechSrf08::GetSensorValue() -> i2c device isn't initialized.\n");
		return API_ERROR;
	}

	unsigned char buf[4];
	if(_i2c->Read(0xE0, buf, 4) < 0) {
		PrintMessage("ERROR : DevantechSrf08::GetSensorValue() -> Can't read from i2c device\n");
		return API_ERROR;
	}

	int distance;
	distance = buf[2] << 8;
	distance |= buf[3];
	sensorValue.resize(1);
	sensorValue[0] = (double)distance * 0.01 / 58.0;

	PrintMessage("INFO : DevantechSrf08::GetSensorValue() -> %.3f\n", sensorValue[0]);

	buf[0] = 0x52;
	if(_i2c->Write(0xE0, buf, 1) != 1) {
		PrintMessage("ERROR : DevantechSrf08::GetSensorValue() -> Can't write i2c device\n");
		return API_ERROR;
	}

	return API_SUCCESS;
}


int DevantechSrf08::InitializeI2C(Property parameter)
{
	if(parameter.FindName("I2cApiName") == false)	return API_ERROR;
	
	if(_i2c != NULL) {
//		PrintMessage("ERROR : DevantechSrf08::InitializeI2C() -> Can't find I2cApiName\n");
		delete _i2c;
		_i2c = NULL;
	}
#if defined(WIN32)
	//	DLL 로드
	hOprosAPI = LoadLibrary((LPCSTR)parameter.GetValue("I2cApiName").c_str());
	if(hOprosAPI == NULL) {
		return API_ERROR;
	}

	GET_OPROS_API getOprosAPI;
	getOprosAPI = (GET_OPROS_API)GetProcAddress(hOprosAPI, "GetAPI");
	if(getOprosAPI == NULL) {
		FreeLibrary(hOprosAPI);
		hOprosAPI = NULL;
		return API_ERROR;
	}
#else
	hOprosAPI = dlopen(parameter.GetValue("I2cApiName").c_str(), RTLD_LAZY);
	if(hOprosAPI == NULL) {
		PrintMessage("ERROR : DevantechSrf08::InitializeI2C -> Can't find the %s\n", parameter.GetValue("I2cApiName").c_str());
		return API_ERROR;
	}

	GET_OPROS_API getOprosAPI;
	getOprosAPI = (GET_OPROS_API)dlsym(hOprosAPI, "GetAPI");
	char *error = dlerror();
	if(error != NULL) {
		PrintMessage("ERROR : DevantechSrf08::InitializeI2C -> Can't get a handle of GetAPI Funtion\n");
		dlclose(hOprosAPI);
		hOprosAPI = NULL;
		return API_ERROR;
	}
#endif

	_i2c = (I2C *)getOprosAPI();
	if(_i2c == NULL) {
#if defined(WIN32)
		FreeLibrary(hOprosAPI);
#else
		dlclose(hOprosAPI);
#endif
		hOprosAPI = NULL;
		return API_ERROR;
	}

	//	API 초기화
	if(_i2c->Initialize(parameter) < 0) {
		FinalizeI2C();
		return API_ERROR;
	}

	if(_i2c->Enable() < 0) {
		FinalizeI2C();
		return API_ERROR;
	}

	return API_SUCCESS;
}

int DevantechSrf08::FinalizeI2C(void)
{
	if(_i2c != NULL) {
		delete _i2c;
		_i2c = NULL;
	}

	if(hOprosAPI != NULL) {
#if defined(WIN32)
		FreeLibrary(hOprosAPI);
#else
		dlclose(hOprosAPI);
#endif
		hOprosAPI = NULL;
	}

	return API_SUCCESS;
}


#if defined(WIN32)
extern "C"
{
__declspec(dllexport) OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new DevantechSrf08();
}
#else
extern "C"
{
	OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new DevantechSrf08();
}
#endif
