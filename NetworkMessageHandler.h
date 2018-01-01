#pragma once
#include "NetworkMessageFragment.h"

class NetworkMessageHandler
{
protected:
	const static uint16_t DEFAULT_HEADERSIZE = 6;
public:
	NetworkMessageHandler();
	virtual ~NetworkMessageHandler();

	virtual void accept(const NetworkMessageFragment& fragment);

	__inline virtual bool isHandlingFinished() const {
		return true;
	}
};

