#include "ROSEServer.h"
#include "ROSEMessageHandler.h"
#include "PacketFactory.h"

std::shared_ptr<CryptTable> ROSEServer::ENCRYPTION_TABLE;

ROSEServer::ROSEServer(uint16_t port) : NetworkServer(port) {
	PacketFactory::init();
}


ROSEServer::~ROSEServer()
{
}

void ROSEServer::loadEncryption() {
	ENCRYPTION_TABLE = std::shared_ptr<CryptTable>(new CryptTable());
	ENCRYPTION_TABLE->generateCryptTables();
}

ROSEClient* ROSEServer::findROSEClientByInterface(NetworkClient* client) {
	ROSEClient* result = nullptr;
	auto iterator = clientList.find(client);
	if (iterator != clientList.cend()) {
		result = (*iterator).second;
	}
	return result;
}

void ROSEServer::onNewClient(NetworkClient* nc) {
	ROSEClient* client = new ROSEClient(nc);
	onNewROSEClient(client);
	clientList.insert(make_pair(nc, client));
}

void ROSEServer::onNewROSEClient(ROSEClient* roseClient) {

}

void ROSEServer::onClientDisconnecting(NetworkClient* nc) {
	ROSEClient* client = findROSEClientByInterface(nc);
	if (client != nullptr) {
		onROSEClientDisconnecting(client);
		clientList.erase(nc);
		delete client;
	}
	client = nullptr;
}

void ROSEServer::onROSEClientDisconnecting(ROSEClient* client) {
	//TODO
}

bool ROSEServer::onPacketsReady(ROSEClient* client, std::queue<std::shared_ptr<Packet>>& packetQueue) {
	return true;
}

void ROSEServer::onPrepareDataIncoming(NetworkClient* nc) {
	ROSEClient* client = findROSEClientByInterface(nc);
	if (client != nullptr) {
		ROSEMessageHandler *handler = dynamic_cast<ROSEMessageHandler*>(client->getMessageHandler());
		uint16_t expectedAmountOfBytes = handler->getExpectedAmountOfBytes();
		std::cout << "Setting the expected amount of bytes to: " << expectedAmountOfBytes << "\n";
		nc->setMaxAllowedBytesToReceive(expectedAmountOfBytes);
	}
}

void ROSEServer::onDataReceived(NetworkClient* nc, const NetworkMessageFragment& dataHolder) {
	ROSEClient* client = findROSEClientByInterface(nc);
	if (client != nullptr) {
		bool messageSuccessfullyHandled = client->handleIncomingDataFragment(dataHolder);
		bool packetSuccessfullyHandled = true;
		if (messageSuccessfullyHandled && client->isPacketQueueFilled()) {
			std::queue<std::shared_ptr<Packet>> packetQueue = client->getQueuedPackets();
			packetSuccessfullyHandled = onPacketsReady(client, packetQueue);
		}
		bool everythingSuccessfullyHandled = messageSuccessfullyHandled && packetSuccessfullyHandled;
		nc->setIsActive(everythingSuccessfullyHandled);
	}
}