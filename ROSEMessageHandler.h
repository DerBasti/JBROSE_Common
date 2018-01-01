#pragma once
#include "NetworkMessageHandler.h"
#include "Trackable.h"
#include "EncryptionHandler.h"
#include "Packet.h"

class ROSEMessageHandler : public NetworkMessageHandler
{
private:
	EncryptionHandler *encryptionHandler;
	uint16_t expectedAmountOfBytes;
	TrackableAppender<char> dataHolder;
	bool handlingFinished;
	bool headerReceived;
public:
	ROSEMessageHandler();
	~ROSEMessageHandler();

	virtual void accept(const NetworkMessageFragment& fragment); 
	void reset();
	std::shared_ptr<Packet> createPacketFromReceivedData();

	__inline void setEncryptionValue(const unsigned long value) {
		encryptionHandler->setEncryptionValue(value);
	}

	__inline virtual bool isHandlingFinished() const {
		return handlingFinished;
	}

	__inline uint16_t getExpectedAmountOfBytes() const {
		return expectedAmountOfBytes;
	}
};

