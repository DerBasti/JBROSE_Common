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
	std::cout << "Packet with ID 0x" << std::hex << packet.getCommandId() << std::dec << " not found!\n";
	return std::shared_ptr<Packet>();
}