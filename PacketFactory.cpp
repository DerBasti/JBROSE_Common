#include <iostream>
#include "PacketFactory.h"

PacketFactory::PacketFactory() {
}


PacketFactory::~PacketFactory() {

}

std::shared_ptr<Packet> PacketFactory::createPacketFromReceivedData(const char *dataPacket) {
	Packet packet = Packet(dataPacket);
	auto iterator = commandToPacketSupplierMap.find(packet.getCommandId());
	if (iterator != commandToPacketSupplierMap.cend()) {
		std::function<std::shared_ptr<Packet>(const Packet&)> packetCreatorFunction = (*iterator).second;
		return packetCreatorFunction(packet);
	}
	logger.logError("Packet with ID ", packet.getCommandIdAsHex(), " not found!");
	return std::shared_ptr<Packet>();
}