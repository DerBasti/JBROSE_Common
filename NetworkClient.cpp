#include "NetworkClient.h"
#include "Ws2tcpip.h"

NetworkClient::NetworkClient(SOCKET socket) {
	this->socket = socket;
	activeFlag = true;
	lastAmountOfBytesReceived = 0;
	maxAllowedBytesToReceive = 0x400;
}

NetworkClient::~NetworkClient() {
	if (socket != INVALID_SOCKET) {
		::closesocket(socket);
	}
	socket = INVALID_SOCKET;
}

void NetworkClient::setIp(const sockaddr_in* addr) {
	char buffer[0x16] = { 0x00 };
	InetNtopA(AF_INET, &addr->sin_addr, buffer, 0x16);
	int length = strlen(buffer);
	ip = std::shared_ptr<char>(new char[length + 1], std::default_delete<char[]>());
	strncpy_s(ip.get(), length+1, buffer, length);
	ip.get()[length] = 0x00;
}

NetworkMessageFragment NetworkClient::receiveData() {
	NetworkMessageFragment dataHolder;
	lastAmountOfBytesReceived = ::recv(this->getSocket(), dataHolder, getMaxAllowedBytesToReceive(), 0x00);
	if (static_cast<int16_t>(lastAmountOfBytesReceived) == -1) {
		lastAmountOfBytesReceived = 0;
	}
	dataHolder.setLength(lastAmountOfBytesReceived);
	return dataHolder;
}

bool NetworkClient::send(const char* str, unsigned int length) {
	bool sendingSuccessful = length == 0;
	if (length > 0 && str != nullptr) {
		int bytesSent = ::send(getSocket(), str, length, 0x00);
		sendingSuccessful = (bytesSent == length);
	}
	return sendingSuccessful;
}