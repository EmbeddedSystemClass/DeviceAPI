#include "DevantechUsbToI2C.h"
#include "OprosPrintMessage.h"

#define	BYTE_ADDRESS_MODE		0
#define	WORD_ADDRESS_MODE		1

#define	I2C_SGL		0x53
#define	I2C_MUL		0x54
#define	I2C_AD1		0x55
#define	I2C_AD2		0x56
#define	I2C_USB		0x5A

DevantechUsbToI2C::DevantechUsbToI2C(void)
{
	_uart = NULL;
	_addressMode = BYTE_ADDRESS_MODE;
}

DevantechUsbToI2C::~DevantechUsbToI2C(void)
{
	Finalize();
}

int DevantechUsbToI2C::Initialize(Property parameter)
{
	if(InitializeUART(parameter) != API_SUCCESS) {
		PrintMessage("ERROR : DevantechUsbToI2C::Initialize() -> Occur a error in InitializeUART().\n");
		return API_ERROR;
	}

	if(SetParameter(parameter) != API_SUCCESS) {
		return API_ERROR;
	}

	return API_SUCCESS;
}

int DevantechUsbToI2C::Finalize(void)
{
	if(_uart != NULL) {
		delete _uart;
		_uart = NULL;
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

int DevantechUsbToI2C::Enable(void)
{
	int revision = GetFirmwareVersion();

	if(revision < 0) {
		PrintMessage("ERROR : DevantechUsbToI2C::Enable() -> Occur a error in GetFirmwareVersion().\n");
	}

	PrintMessage("INFO : DevantechUsbToI2C::Enable() -> Firmware Version : %d\n", revision);

	return API_SUCCESS;
}

int DevantechUsbToI2C::Disable(void)
{
	return API_SUCCESS;
}

int DevantechUsbToI2C::SetParameter(Property parameter)
{
	if(parameter.FindName("AddressMode") == false) {
		PrintMessage("ERROR : DevantechUsbToI2C::SetParameter() -> Can't find parameters\n");
	}
	_addressMode = atoi(parameter.GetValue("AddressMode").c_str());
	if(!(_addressMode == BYTE_ADDRESS_MODE || _addressMode == WORD_ADDRESS_MODE)) {
		return API_ERROR;
	}

	
	this->parameter = parameter;

	return API_SUCCESS;
}

int DevantechUsbToI2C::GetParameter(Property &parameter)
{
	parameter = this->parameter;

	return API_SUCCESS;
}

int DevantechUsbToI2C::Lock(void)
{
	_lock.Lock();

	return API_SUCCESS;
}

int DevantechUsbToI2C::Unlock(void)
{
	_lock.Unlock();

	return API_SUCCESS;
}

int DevantechUsbToI2C::Write(unsigned long address, unsigned char *data, int size)
{
	if(_addressMode == BYTE_ADDRESS_MODE) {
		unsigned char addr = (unsigned char)address;
		return WriteUSBtoI2C(addr, data, size);
	}
	else {	//	WORD_ADDRESS_MODE
		unsigned short addr = (unsigned char)address;
		return WriteUSBtoI2C(addr, data, size);
	}

	PrintMessage("ERROR : DevantechUsbToI2C::Write()\n");
	return API_ERROR;
}

int DevantechUsbToI2C::Read(unsigned long address, unsigned char *data, int size)
{
	if(_addressMode == BYTE_ADDRESS_MODE) {
		unsigned char addr = (unsigned char)address;
		return ReadUSBtoI2C(addr, data, size);
	}
	else {	//	WORD_ADDRESS_MODE
		unsigned short addr = (unsigned char)address;
		return ReadUSBtoI2C(addr, data, size);
	}

	PrintMessage("ERROR : DevantechUsbToI2C::Read()\n");
	return API_ERROR;
}

int DevantechUsbToI2C::InitializeUART(Property parameter)
{
	Property uartParameter;
	char *error = NULL;

	if(parameter.FindName("UartApiName") == false)	goto ERROR_Devantech_USBtoI2C_InitializeUART;
	if(parameter.FindName("PortName") == false)		goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("PortName", parameter.GetValue("PortName"));
	if(parameter.FindName("TimeOut") == false)		goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("TimeOut", parameter.GetValue("TimeOut"));
	if(parameter.FindName("BaudRate") == false)		goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("BaudRate", parameter.GetValue("BaudRate"));
	if(parameter.FindName("DataBits") == false)		goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("DataBits", parameter.GetValue("DataBits"));
	if(parameter.FindName("StopBits") == false)		goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("StopBits", parameter.GetValue("StopBits"));
	if(parameter.FindName("Parity") == false)		goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("Parity", parameter.GetValue("Parity"));
	if(parameter.FindName("FlowControl") == false)	goto ERROR_Devantech_USBtoI2C_InitializeUART;
	else	uartParameter.SetValue("FlowControl", parameter.GetValue("FlowControl"));

	if(_uart != NULL) {
		delete _uart;
		_uart = NULL;
	}
#if defined(WIN32)
	//	DLL 로드
	hOprosAPI = LoadLibrary((LPCSTR)parameter.GetValue("UartApiName").c_str());
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
	hOprosAPI = dlopen(parameter.GetValue("UartApiName").c_str(), RTLD_LAZY);
	if(hOprosAPI == NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::InitializeUART() -> Can't find the %s\n", parameter.GetValue("UartApiName").c_str());
		return API_ERROR;
	}

	GET_OPROS_API getOprosAPI;
	getOprosAPI = (GET_OPROS_API)dlsym(hOprosAPI, "GetAPI");
	error = dlerror();
	if(error != NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::InitializeUART() -> Can't get a handle of GetAPI Funtion\n");
		dlclose(hOprosAPI);
		hOprosAPI = NULL;
		return API_ERROR;
	}
#endif

	_uart = (Uart *)getOprosAPI();
	if(_uart == NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::InitializeUART() -> Can't get a handle of Uart API\n");
		Finalize();
		return API_ERROR;
	}

	//	API 초기화
	if(_uart->Initialize(uartParameter) < 0) {
		PrintMessage("ERROR : DevantechUsbToI2C::InitializeUART() -> Can't initialize Uart\n");
		Finalize();
		return API_ERROR;
	}

	if(_uart->Enable() < 0) {
		PrintMessage("ERROR : DevantechUsbToI2C::InitializeUART() -> Can't enable Uart\n");
		Finalize();
		return API_ERROR;
	}

	return API_SUCCESS;

ERROR_Devantech_USBtoI2C_InitializeUART :
	PrintMessage("ERROR : DevantechUsbToI2C::InitializeUART() -> Can't find a property in parameter.\n");
	return API_ERROR;
}

int DevantechUsbToI2C::GetFirmwareVersion(void)
{
	unsigned char buf[4] = {I2C_USB, 0x01};

	Lock();

	if(_uart->Write(buf, sizeof(buf)) != sizeof(buf)) {
		PrintMessage("ERROR : DevantechUsbToI2C::GetFirmwareVersion() -> Can't write uart\n");
		Unlock();
		return API_ERROR;
	}

	if(_uart->Read(buf, 1) != 1) {
		PrintMessage("ERROR : DevantechUsbToI2C::GetFirmwareVersion() -> Can't read uart\n");
		Unlock();
		return API_ERROR;
	}

	Unlock();

	return (int)buf[0];
}

int DevantechUsbToI2C::WriteUSBtoI2C(unsigned char addr, unsigned char *data, int size)
{
	unsigned char buf[] = {I2C_AD1, addr, 0x00, 0x00};

	if(_uart == NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart is not initialized\n");
		return -1;
	}

	if(size < 0 || size > 255) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Size is not valid\n");
		return -1;
	}

	buf[3] = (unsigned char)size;

	if(_uart->Write(buf, sizeof(buf)) != sizeof(buf)) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart write error!\n");
		return -1;
	}

	if(_uart->Write(data, size) != size) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart write error!\n");
		return -1;
	}

	if(_uart->Read(data, 1) != 1) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart read error!\n");
		return -1;
	}

	return size;
}

int DevantechUsbToI2C::WriteUSBtoI2C(unsigned short addr, unsigned char *data, int size)
{
	unsigned char buf[] = {I2C_AD1, 0xA0, (addr >> 8), (addr & 0xFF), 0x00};

	if(_uart == NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart is not initialized\n");
		return API_ERROR;
	}

	if(size < 0 || size > 255) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Size is not valid\n");
		return API_ERROR;
	}

	buf[3] = (unsigned char)size;

	if(_uart->Write(buf, sizeof(buf)) != sizeof(buf)) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart write error!\n");
		return API_ERROR;
	}

	if(_uart->Write(data, size) != size) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart write error!\n");
		return API_ERROR;
	}

	if(_uart->Read(data, 1) != 1) {
		PrintMessage("ERROR : DevantechUsbToI2C::WriteUSBtoI2C() -> Uart read error!\n");
		return API_ERROR;
	}

	return size;
}

int DevantechUsbToI2C::ReadUSBtoI2C(unsigned char addr, unsigned char *data, int size)
{
	unsigned char buf[] = {I2C_AD1, addr + 1, 0x00, 0x00};

	if(_uart == NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Uart is not initialized\n");
		return API_ERROR;
	}

	if(size < 0 || size > 255) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Size is not valid\n");
		return API_ERROR;
	}

	buf[3] = (unsigned char)size;

	if(_uart->Write(buf, sizeof(buf)) != sizeof(buf)) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Uart write error!\n");
		return API_ERROR;
	}

	if(_uart->Read(data, size) != size) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Uart read %d data error!\n", size);
		return API_ERROR;
	}

	return size;
}

int DevantechUsbToI2C::ReadUSBtoI2C(unsigned short addr, unsigned char *data, int size)
{
	unsigned char buf[] = {I2C_AD1, 0xA1, (addr >> 8), (addr & 0xFF), 0x00};

	if(_uart == NULL) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Uart is not initialized\n");
		return API_ERROR;
	}

	if(size < 0 || size > 255) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Size is not valid\n");
		return API_ERROR;
	}

	buf[3] = (unsigned char)size;

	if(_uart->Write(buf, sizeof(buf)) != sizeof(buf)) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Uart write error!\n");
		return API_ERROR;
	}

	if(_uart->Read(data, size) != size) {
		PrintMessage("ERROR : DevantechUsbToI2C::ReadUSBtoI2C() -> Uart read error!\n");
		return API_ERROR;
	}

	return size;
}


#if defined(WIN32)
extern "C"
{
__declspec(dllexport) OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new DevantechUsbToI2C();
}
#else
extern "C"
{
	OprosApi *GetAPI();
}

OprosApi *GetAPI()
{
	return new DevantechUsbToI2C();
}
#endif
