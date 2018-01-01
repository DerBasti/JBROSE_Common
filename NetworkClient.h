#pragma once

#include <winsock2.h>
#include <memory>
#include <algorithm>
#include <queue>
#include "NetworkMessageFragment.h"

class NetworkClient {
private:
	SOCKET socket;
	std::shared_ptr<char> ip;
	bool activeFlag; 
	uint16_t lastAmountOfBytesReceived;
	uint16_t maxAllowedBytesToReceive;
public:
	NetworkClient(SOCKET socket);
	virtual ~NetworkClient();

	NetworkMessageFragment receiveData();

	__inline bool send(const char* str) {
		return send(str, str != nullptr ? static_cast<unsigned int>(strlen(str)) : 0);
	}

	bool send(const char* str, unsigned int length);

	void setIp(const sockaddr_in* addr);

	__inline const char* getIp() const {
		return ip.get();
	}

	__inline uint16_t getMaxAllowedBytesToReceive() const {
		return maxAllowedBytesToReceive;
	}
	__inline void setMaxAllowedBytesToReceive(uint16_t max) {
		maxAllowedBytesToReceive = max;
	}

	__inline bool isActive() const {
		return socket != INVALID_SOCKET && activeFlag;
	}

	__inline void setIsActive(const bool flag) {
		activeFlag = flag;
	}

	__inline SOCKET getSocket() const {
		return socket;
	}
};


