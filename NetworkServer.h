#ifndef __NETWORK_SERVER__
#define __NETWORK_SERVER__
#pragma once

#include <winsock2.h>
#include <iostream>
#include <set>
#include <thread>

#include "NetworkClient.h"

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

class WSADataStartup {
public:
	WSADataStartup() {
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);
	}
	~WSADataStartup() {
		WSACleanup();
	}
};


class NetworkServer {
private:
	WSADataStartup wsaStartup;
	SOCKET socket;
	fd_set fds;
	unsigned int maxFileDescriptors;
	uint16_t port;
	std::set<NetworkClient*> clientList;
	class ROSELogger logger;

	bool startServerConnection();
	bool bindSocket() const;
	bool startListeningOnSocket() const;
	void prepareForIncomingData();
	void prepareClientForIncomingData(NetworkClient* nc);
	void acceptAndAddNewClient();
	void addNewClient(SOCKET clientSocket, sockaddr_in* clientInfo);
	void handleConnectedClients();
	bool handleIncomingMessages();

	__inline bool establishSocket() {
		socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		return socket != INVALID_SOCKET;
	}

	__inline SOCKET getSocket() const {
		return socket;
	}
protected:
	void disconnectClient(NetworkClient* nc);
	virtual void onServerStartup() {

	}
	virtual void onNewClient(NetworkClient* nc) {
	}

	virtual void onPrepareDataIncoming(NetworkClient* nc) {
	}

	virtual void onDataReceived(NetworkClient* nc, const NetworkMessageFragment& dataHolder);

	virtual void onClientDisconnecting(NetworkClient* nc) {
		cout << "Disconnecting client " << nc->getIp() << "\n";
	}
public:
	NetworkServer(uint16_t port);
	virtual ~NetworkServer();
	virtual void startAcceptingClients() final;
};


#endif //__NETWORK_SERVER__
