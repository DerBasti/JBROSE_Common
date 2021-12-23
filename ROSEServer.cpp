#include "ROSEServer.h"
#include "ROSEMessageHandler.h"
#include "PacketFactory.h"
#include "MySQLDatabase.h"
#include "PostgresDatabase.h"

std::shared_ptr<CryptTable> ROSEServer::ENCRYPTION_TABLE;
ROSEServer* ROSEServer::instance;

ROSEServer::ROSEServer(uint16_t port) : NetworkServer(port) {
	instance = this;

	srand(static_cast<uint32_t>(time(nullptr)));

	DatabaseConnectionInformation info;
	info.setPort(5432);
	info.setUserName("postgres");
	info.setPassword("bla123");
	info.setDatabaseName("rose_online");
	logger.logDebug("Database connection to: ", info.getHostName(), ":", info.getPort(), "/", info.getDatabaseName());
	database = new PostgresDatabase(info);
}


ROSEServer::~ROSEServer()
{
	delete database;
}

void ROSEServer::loadEncryption() {
	ENCRYPTION_TABLE = std::shared_ptr<CryptTable>(new CryptTable());
	ENCRYPTION_TABLE->generateCryptTables();
}

std::shared_ptr<ROSEClient> ROSEServer::findROSEClientByInterface(NetworkClient* client) {
	std::shared_ptr<ROSEClient> result;
	auto iterator = clientList.find(client);
	if (iterator != clientList.cend()) {
		result = (*iterator).second;
	}
	return result;
}

void ROSEServer::onNewClient(NetworkClient* nc) {
	auto client = std::shared_ptr<ROSEClient>(new ROSEClient(nc, packetFactoryCreatorFunction));
	onNewROSEClient(client);
	clientList.insert(make_pair(nc, client));
}

void ROSEServer::onNewROSEClient(std::shared_ptr<ROSEClient>& roseClient) {

}

void ROSEServer::onClientDisconnecting(NetworkClient* nc) {
	auto client = findROSEClientByInterface(nc);
	if (client != nullptr) {
		onROSEClientDisconnecting(client);
		clientList.erase(nc);
	}
}

void ROSEServer::onROSEClientDisconnecting(std::shared_ptr<ROSEClient>& client) {
	//TODO
}

bool ROSEServer::onPacketsReady(std::shared_ptr<ROSEClient>& client, std::queue<std::shared_ptr<Packet>>& packetQueue) {
	return true;
}

void ROSEServer::onPrepareDataIncoming(NetworkClient* nc) {
	std::shared_ptr<ROSEClient> client = findROSEClientByInterface(nc);
	if (client) {
		ROSEMessageHandler *handler = dynamic_cast<ROSEMessageHandler*>(client->getMessageHandler());
		uint16_t expectedAmountOfBytes = handler->getExpectedAmountOfBytes();
		logger.logTrace("Setting the expected amount of bytes to: ", expectedAmountOfBytes);
		nc->setMaxAllowedBytesToReceive(expectedAmountOfBytes);
	}
}

void ROSEServer::onDataReceived(NetworkClient* nc, const NetworkMessageFragment& dataHolder) {
	std::shared_ptr<ROSEClient> client = findROSEClientByInterface(nc);
	if (client) {
		bool messageSuccessfullyHandled = client->handleIncomingDataFragment(dataHolder);
		bool packetSuccessfullyHandled = true;
		if (messageSuccessfullyHandled && client->isPacketQueueFilled()) {
			std::queue<std::shared_ptr<Packet>> packetQueue = std::move(client->getQueuedPackets());
			logger.logTrace("Client from ", nc->getIp(), " has ", packetQueue.size(), " packets in queue.");
			packetSuccessfullyHandled = onPacketsReady(client, packetQueue);
		}
		bool everythingSuccessfullyHandled = messageSuccessfullyHandled && packetSuccessfullyHandled;
		nc->setIsActive(everythingSuccessfullyHandled);
	}
}