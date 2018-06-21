#include "IdentifyAccountRequestPacket.h"

IdentifyAccountRequestPacket::IdentifyAccountRequestPacket(const Packet* packet) : Packet(IdentifyAccountRequestPacket::ID, IdentifyAccountRequestPacket::DEFAULT_SIZE) {
	accountId = *(uint32_t*)packet->getRawData();
}

IdentifyAccountRequestPacket::~IdentifyAccountRequestPacket() {

}