#pragma once

#include "I2C.h"
#include "Uart.h"
#include "OprosLock.h"

#if defined(WIN32)
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

class DevantechUsbToI2C : public I2C
{
public:
	DevantechUsbToI2C(void);
	virtual ~DevantechUsbToI2C(void);

public:
	int Initialize(Property parameter);
	int Finalize(void);
	int Enable(void);
	int Disable(void);
	int SetParameter(Property parameter);
	int GetParameter(Property &parameter);
	int Lock(void);
	int Unlock(void);
	int Write(unsigned long address, unsigned char *data, int size);
	int Read(unsigned long address, unsigned char *data, int size);

private:
	Uart *_uart;
	OprosLock _lock;

#if defined(WIN32)
	HMODULE hOprosAPI;
#else
	void *hOprosAPI;
#endif
	int InitializeUART(Property parameter);

	int _addressMode;

	int GetFirmwareVersion(void);

	int WriteUSBtoI2C(unsigned char addr, unsigned char *data, int size);
	int WriteUSBtoI2C(unsigned short addr, unsigned char *data, int size);
	int ReadUSBtoI2C(unsigned char addr, unsigned char *data, int size);
	int ReadUSBtoI2C(unsigned short addr, unsigned char *data, int size);
};

