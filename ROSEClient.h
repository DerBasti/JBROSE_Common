#pragma once
#include <mutex>
#include "NetworkClient.h"
#include "NetworkMessageHandler.h"
#include "Packet.h"

class ROSEClient
{
private:

	void onEncryptionOfPacket(const std::string& packetToPrintable, const SendablePacket& packet) const;
protected:
	const static uint16_t DEFAULT_HEADERSIZE = 6;
	std::queue<std::shared_ptr<Packet>> packetQueue;
	std::mutex packetQueueMutex;
	NetworkClient* networkInterface;
	std::unique_ptr<NetworkMessageHandler> messageHandlerForIncomingData;
	
	__inline NetworkClient* getNetworkInterface() const {
		return networkInterface;
	} 

	void threadSafeAppendingOfPacketToQueue(std::shared_ptr<Packet>& packet) {
		std::lock_guard<std::mutex> mutexLock(packetQueueMutex);
		packetQueue.push(packet);
	}
	void resetToDefaultState();
public:
	ROSEClient(NetworkClient* networkInterface);
	virtual ~ROSEClient();

	bool handleIncomingDataFragment(const NetworkMessageFragment& data);

	NetworkMessageHandler* getMessageHandler() const {
		return messageHandlerForIncomingData.get();
	}

	std::queue<std::shared_ptr<Packet>> getQueuedPackets();
	bool sendData(const ResponsePacket& packet) const;

	__inline bool isPacketQueueFilled() {
		std::lock_guard<std::mutex> mutexLock(packetQueueMutex);
		return !packetQueue.empty();
	}
};

