#pragma once
#include "Packet.h"
#include <functional>
#include <unordered_map>
#include <memory>

class PacketFactory
{
private:
	PacketFactory();
	static std::unordered_map<uint16_t, std::function<std::shared_ptr<Packet>(const Packet&)> > commandToPacketSupplierMap;
public:
	~PacketFactory();
	static void init();
	static std::shared_ptr<Packet> createPacketFromReceivedData(const char *dataPacket);
};

