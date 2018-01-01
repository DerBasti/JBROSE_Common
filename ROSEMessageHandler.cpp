#include "ROSEMessageHandler.h"
#include "ROSEServer.h"
#include "PacketFactory.h"

ROSEMessageHandler::ROSEMessageHandler() : NetworkMessageHandler() {
	expectedAmountOfBytes = DEFAULT_HEADERSIZE;
	this->encryptionHandler = new EncryptionHandler();
	headerReceived = false;
}


ROSEMessageHandler::~ROSEMessageHandler() {
	encryptionHandler = nullptr;
}

void ROSEMessageHandler::accept(const NetworkMessageFragment& fragment) {
	dataHolder.add(fragment.getMessage(), fragment.getLength());
	expectedAmountOfBytes -= fragment.getLength();
	std::cout << "Header of length " << fragment.getLength() << " received.\n";
	if (dataHolder.getCurrentlyUsedSize() == DEFAULT_HEADERSIZE && !headerReceived) {
		std::cout << "Only Header received. Trying to decrypt it...\n";
		uint16_t newExpectedLengthInTotal = encryptionHandler->decryptHeader((unsigned char*)dataHolder.get());
		std::cout << "New expected length: " << newExpectedLengthInTotal << "\n";
		expectedAmountOfBytes = newExpectedLengthInTotal - DEFAULT_HEADERSIZE;
		headerReceived = true;
	}
	if (expectedAmountOfBytes > 0) {
		std::cout << "Remaining bytes: " << expectedAmountOfBytes << "\n";
		return;
	}
	std::cout << "Decrypting data...\n";
	unsigned char *buffer = (unsigned char*)dataHolder.get();
	bool success = encryptionHandler->decryptBuffer(buffer);
	handlingFinished = true;
}

std::shared_ptr<Packet> ROSEMessageHandler::createPacketFromReceivedData() {
	const char *packetData = (const char*)dataHolder;
	std::shared_ptr<Packet> p = PacketFactory::createPacketFromReceivedData(packetData);
	return p;
}

void ROSEMessageHandler::reset() {
	dataHolder.reset();
	expectedAmountOfBytes = DEFAULT_HEADERSIZE;
	handlingFinished = headerReceived = false;
}