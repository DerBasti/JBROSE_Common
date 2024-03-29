#include "ROSEClient.h"
#include "ROSEMessageHandler.h"
#include <iostream>

ROSEClient::ROSEClient(NetworkClient* networkInterface, std::function<std::shared_ptr<PacketFactory>()> packetFactoryCreator) {
	this->networkInterface = networkInterface;
	std::shared_ptr<PacketFactory> factory = packetFactoryCreator();
	messageHandlerForIncomingData = std::unique_ptr<NetworkMessageHandler>(new ROSEMessageHandler(factory));
}


ROSEClient::~ROSEClient()
{
	networkInterface = nullptr;
}

bool ROSEClient::handleIncomingDataFragment(const NetworkMessageFragment& data) {
	logger.logTrace("Incoming data of size: ", data.getLength());
	messageHandlerForIncomingData->accept(data);
	if (messageHandlerForIncomingData->isHandlingFinished()) {
		logger.logTrace("Handling is finished. Creating packet from data...");
		ROSEMessageHandler *roseMessageHandler = dynamic_cast<ROSEMessageHandler*>(messageHandlerForIncomingData.get());
		std::shared_ptr<Packet> packet = roseMessageHandler->createPacketFromReceivedData();
		if (!packet) {
			logger.logError("Packet creation failed!");
			return false;
		}
		threadSafeAppendingOfPacketToQueue(packet);
		resetToDefaultState();
	}
	return true;
}

void ROSEClient::resetToDefaultState() {
	logger.logTrace("Resetting message handler to default state...");
	ROSEMessageHandler *roseMessageHandler = dynamic_cast<ROSEMessageHandler*>(messageHandlerForIncomingData.get());
	roseMessageHandler->reset();
	getNetworkInterface()->setMaxAllowedBytesToReceive(roseMessageHandler->getExpectedAmountOfBytes());
}

std::queue<std::shared_ptr<Packet>> ROSEClient::getQueuedPackets() {
	std::queue<std::shared_ptr<Packet>> resultQueue;
	std::lock_guard<std::mutex> mutexLock(packetQueueMutex);
	resultQueue.swap(packetQueue);
	return resultQueue;
}

bool ROSEClient::sendData(const ResponsePacket& packet) const {
	SendablePacket sendablePacket = packet.toSendable();
	std::shared_ptr<unsigned char> packetData = sendablePacket.toSendable();
	unsigned char *packetAsPointer = packetData.get();

	std::string packetContentPrettyFormatted = packet.toPrintable();
	//onEncryptionOfPacket(packetContentPrettyFormatted, sendablePacket);

	EncryptionHandler encrypter;
	encrypter.encryptBuffer(packetAsPointer);
	const char* encryptedPacket = (const char*)packetAsPointer;

	unsigned long actualLength = sendablePacket.getCurrentSize();
	logger.logTrace("Sending packet (", actualLength, " bytes) ", packet.toPrintable().c_str());
	bool successfullySent = getNetworkInterface()->send(encryptedPacket, actualLength);
	return successfullySent;
}

void ROSEClient::onEncryptionOfPacket(const std::string& packetToPrintable, const SendablePacket& packet) const {
	FILE *fh = nullptr;
	fopen_s(&fh, "D:\\Games\\ROSE Server\\PacketLog.txt", "a+");
	char buffer[0x100] = { 0x00 };
	sprintf_s(buffer, "==========\r\n%s\r\n===========\r\nData:\r\n", packetToPrintable.c_str());
	fputs(buffer, fh);
	memset(buffer, 0x00, 0x100);
	
	uint16_t length = packet.getCurrentSize();
	unsigned char *packetAsPointer = packet.toSendable().get();
	for (unsigned int i = 0; i < length; i++) {
		sprintf_s(buffer, " %02x", *packetAsPointer);
		packetAsPointer++;
		fputs(buffer, fh);
		if (i % 10 == 0 && i > 0) {
			fputs("\r\n", fh);
		}
	}
	fputs("\r\n\r\n\r\n", fh);
	fclose(fh);
}
