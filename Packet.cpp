#include "Packet.h"
#include <iostream>

SendablePacket::SendablePacket(const Packet& packet) {
	unsigned char *buffer = new unsigned char[0x400];
	memset(buffer, 0x00, packet.getLength());
	sharedBuffer = std::shared_ptr<unsigned char>(buffer, std::default_delete<unsigned char[]>());
}

Packet::Packet() {
	length = commandId = 0;
}

Packet::Packet(const char *data)
{
	const uint16_t *usData = reinterpret_cast<const uint16_t*>(data);
	length = usData[0];
	commandId = usData[1];
	if (length > DEFAULT_MINIMUM_SIZE) {
		rawData = std::shared_ptr<char>(new char[length - DEFAULT_MINIMUM_SIZE], std::default_delete<char[]>());
		memcpy(rawData.get(), &data[DEFAULT_MINIMUM_SIZE], length - DEFAULT_MINIMUM_SIZE);
	}
}


Packet::~Packet()
{
}

SendablePacket ResponsePacket::toSendable() const {
	SendablePacket packet(*this);
	appendHeaderToSendable(packet);
	appendContentToSendable(packet);
	packet.updatePacketSize();
	return packet;
}