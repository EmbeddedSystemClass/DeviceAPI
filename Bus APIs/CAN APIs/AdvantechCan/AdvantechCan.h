#pragma once

#include "Can.h"
#include "AdvantechCanImpl.h"

class AdvantechCan : public Can
{
public:
	AdvantechCan(void);
	virtual ~AdvantechCan(void);

private:
	int _channel;
	bool _isInitialized;
	bool _isOpened;

public:
	int Initialize(Property parameter);
	int Finalize(void);
	int Enable(void);
	int Disable(void);
	int SetParameter(Property parameter);
	int GetParameter(Property &parameter);

	int Lock(void);
	int Unlock(void);

	int Write(long id, unsigned char *data, int size);
	int Read(long &id, unsigned char *data, int size);
};

