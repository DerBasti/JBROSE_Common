#include <iostream>
#include "PacketFactory.h"
#include "..\LoginPackets\RequestPackets.h"

std::unordered_map<uint16_t, std::function<std::shared_ptr<Packet>(const Packet&)>> PacketFactory::commandToPacketSupplierMap;

PacketFactory::PacketFactory() {

}


PacketFactory::~PacketFactory() {

}

std::shared_ptr<Packet> PacketFactory::createPacketFromReceivedData(const char *dataPacket) {
	Packet packet = Packet(dataPacket);
	auto iterator = PacketFactory::commandToPacketSupplierMap.find(packet.getCommandId());
	if (iterator != PacketFactory::commandToPacketSupplierMap.cend()) {
		std::function<std::shared_ptr<Packet>(const Packet&)> packetCreatorFunction = (*iterator).second;
		return packetCreatorFunction(packet);
	}
	std::cout << "Packet with ID 0x" << std::hex << packet.getCommandId() << std::dec << " not found!\n";
	return std::shared_ptr<Packet>();
}

void PacketFactory::init() {
#define ADD_PACKET_TO_HANDLE(id, PacketClass) \
	commandToPacketSupplierMap.insert(std::make_pair(id, [](const Packet& p) { \
		return std::shared_ptr<Packet>(new PacketClass(&p)); \
	}))

	ADD_PACKET_TO_HANDLE(0x703, EncryptionRequestPacket);
	ADD_PACKET_TO_HANDLE(0x704, ServerListRequestPacket);
	ADD_PACKET_TO_HANDLE(0x708, UserLoginRequestPacket);
	ADD_PACKET_TO_HANDLE(0x70A, ChannelServerRequestPacket);
}