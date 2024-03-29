#include "NetworkServer.h"
#include "Logger.h"

NetworkServer::NetworkServer(uint16_t port) {
	ROSEThreadedLogger::init();
	this->port = port;
}

NetworkServer::~NetworkServer() {
	for (NetworkClient *nc : clientList) {
		disconnectClient(nc);
	}
	if (socket != INVALID_SOCKET) {
		::closesocket(socket);
	}
	socket = INVALID_SOCKET;
}

bool NetworkServer::startServerConnection() {
	bool socketValid = establishSocket();
	bool socketBound = socketValid ? bindSocket() : false;
	bool listeningOnSocket = socketBound ? startListeningOnSocket() : false;
	return socketValid && socketBound && listeningOnSocket;
}

bool NetworkServer::bindSocket() const {
	sockaddr_in sockAddr = { 0x00 };
	sockAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	sockAddr.sin_port = htons(port);
	sockAddr.sin_family = AF_INET;
	int result = ::bind(socket, (const sockaddr*)&sockAddr, sizeof(sockaddr_in));
	return result == 0;
}

bool NetworkServer::startListeningOnSocket() const {
	unsigned int optVal = 1;
	::setsockopt(getSocket(), SOL_SOCKET, SO_KEEPALIVE, (const char*)&optVal, sizeof(optVal));

	//Verhindern, dass zu kleine Pakete nicht gesendet/empfangen werden. ROSE hat viele kleinere.
	::setsockopt(getSocket(), IPPROTO_TCP, TCP_NODELAY, (const char*)&optVal, sizeof(optVal));
	return ::listen(socket, SOMAXCONN) == 0;
}

void NetworkServer::prepareForIncomingData() {
	FD_ZERO(&fds);
	for (auto it = clientList.cbegin(); it != clientList.cend();) {
		NetworkClient* nc = *it;
		it++;
		prepareClientForIncomingData(nc);
	}
	FD_SET(getSocket(), &fds);
}

void NetworkServer::prepareClientForIncomingData(NetworkClient* nc) {
	if (nc->isActive()) {
		FD_SET(nc->getSocket(), &fds);
		if (nc->getSocket() > maxFileDescriptors) {
			maxFileDescriptors = nc->getSocket();
		}
	}
	else {
		disconnectClient(nc);
	}
}

void NetworkServer::acceptAndAddNewClient() {
	sockaddr_in clientInfo{ 0x00 };
	int clientinfolen = sizeof(sockaddr_in);
	SOCKET newClientSocket = ::accept(this->getSocket(), (sockaddr*)&clientInfo, (int*)&clientinfolen);
	if (newClientSocket != INVALID_SOCKET) {
		addNewClient(newClientSocket, &clientInfo);
	}
}

void NetworkServer::addNewClient(SOCKET clientSocket, sockaddr_in* clientInfo) {
	NetworkClient* nc = new NetworkClient(clientSocket);
	nc->setIp(clientInfo);
	clientList.insert(nc);
	onNewClient(nc);
	logger.logInfo("New client connected from: '", nc->getIp(), "'.");
}

void NetworkServer::handleConnectedClients() {
	std::set<NetworkClient*> clientsToDisconnect;
	std::for_each(clientList.cbegin(), clientList.cend(), [this, &clientsToDisconnect](NetworkClient* client) {
		if (client->isActive() && FD_ISSET(client->getSocket(), &fds)) {
			onPrepareDataIncoming(client);
			NetworkMessageFragment fragment = client->receiveData();
			client->setIsActive(fragment.getLength() > 0);
			if (client->isActive()) {
				onDataReceived(client, fragment);
			}
			else {
				clientsToDisconnect.insert(client);
			}
		}
	});
	for (NetworkClient* nc : clientsToDisconnect) {
		disconnectClient(nc);
	}
}

bool NetworkServer::handleIncomingMessages() {
	static timeval timeout{ 0, 1000 };
	bool wasAnythingSentToServer = false;
	int activity = ::select(maxFileDescriptors + 1, &fds, nullptr, nullptr, &timeout);
	if (activity && FD_ISSET(this->getSocket(), &fds)) {
		wasAnythingSentToServer = true;
		acceptAndAddNewClient();
	}
	return wasAnythingSentToServer;
}

void NetworkServer::onDataReceived(NetworkClient* nc, const NetworkMessageFragment& dataHolder) {
	logger.logDebug("Client from IP '", nc->getIp(), "' sent: ", dataHolder.getMessage());
}

void NetworkServer::disconnectClient(NetworkClient* nc) {
	logger.logInfo("Client from IP '", nc->getIp(), "' will disconnect now.");
	onClientDisconnecting(nc);
	clientList.erase(nc);
	delete nc;
	nc = nullptr;
}

void NetworkServer::startAcceptingClients() {
	logger.logDebug("Starting to accept clients...");
	onServerStartup();
	startServerConnection();
	do {
		prepareForIncomingData();
		handleIncomingMessages();
		handleConnectedClients();
	} while (true);
}
