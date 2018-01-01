#include "ROSEClient.h"
#include "ROSEMessageHandler.h"
#include <iostream>

ROSEClient::ROSEClient(NetworkClient* networkInterface) {
	this->networkInterface = networkInterface;
	messageHandlerForIncomingData = std::unique_ptr<NetworkMessageHandler>(new ROSEMessageHandler());
}


ROSEClient::~ROSEClient()
{
	networkInterface = nullptr;
}

bool ROSEClient::handleIncomingDataFragment(const NetworkMessageFragment& data) {
	std::cout << "Incoming data of size: " << data.getLength() << "\n";
	messageHandlerForIncomingData->accept(data);
	if (messageHandlerForIncomingData->isHandlingFinished()) {
		std::cout << "Handling is finished. Creating packet from data...\n";
		ROSEMessageHandler *roseMessageHandler = dynamic_cast<ROSEMessageHandler*>(messageHandlerForIncomingData.get());
		std::shared_ptr<Packet> packet = roseMessageHandler->createPacketFromReceivedData();
		if (!packet) {
			std::cout << "Packet creation failed!\n";
			return false;
		}
		threadSafeAppendingOfPacketToQueue(packet);
		resetToDefaultState();
	}
	return true;
}

void ROSEClient::resetToDefaultState() {
	std::cout << "Resetting to default state...\n";
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
	std::cout << "Sending packet " << packet.toPrintable().c_str() << "\n";
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
