#pragma once
#include "Packet.h"
#include "Logger.h"
#include <functional>
#include <unordered_map>
#include <memory>

class PacketFactory
{
private:
	std::unordered_map<uint16_t, std::function<std::shared_ptr<Packet>(const Packet&)> > commandToPacketSupplierMap;
protected:
	ROSELogger logger;
	template<class T, class = typename std::enable_if<std::is_base_of<Packet, T>::value>::type>
	__inline void addCommand(uint16_t command) {
		commandToPacketSupplierMap.insert(std::make_pair(command, [](const Packet& p) {
			return std::shared_ptr<Packet>(new T(&p));
		}));
	}

public:
	PacketFactory();
	~PacketFactory();
	std::shared_ptr<Packet> createPacketFromReceivedData(const char *dataPacket);
};