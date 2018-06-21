#pragma once
#include "..\Packet.h"

class IdentifyAccountRequestPacket : public Packet {
private:
	const static uint16_t DEFAULT_SIZE = 10;
	uint32_t accountId;
public:
	const static uint16_t ID = 0x70B;
	IdentifyAccountRequestPacket(const Packet* packet);
	virtual ~IdentifyAccountRequestPacket();

	__inline uint32_t getAccountId() const {
		return accountId;
	}

	virtual std::string toPrintable() const {
		char buf[0x100] = { 0x00 };
		sprintf_s(buf, "[IdentifyAccountRequest - %i bytes]", getLength());
		return std::string(buf);
	}
};