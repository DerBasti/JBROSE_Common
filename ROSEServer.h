#pragma once
#include "NetworkServer.h"
#include "ROSEClient.h"
#include "EncryptionHandler.h"
#include <unordered_map>
class ROSEServer : public NetworkServer
{
private:
	std::unordered_map<NetworkClient*, ROSEClient*> clientList;
	ROSEClient* findROSEClientByInterface(NetworkClient* client);
protected:
	static std::shared_ptr<CryptTable> ENCRYPTION_TABLE;
	virtual void loadEncryption();
public:
	ROSEServer(uint16_t port);
	~ROSEServer();

	__inline static const CryptTable* getEncryptionTable() {
		return ENCRYPTION_TABLE.get();
	}
	virtual void onPrepareDataIncoming(NetworkClient* nc);
	void onDataReceived(NetworkClient* nc, const NetworkMessageFragment& dataHolder) final;
	virtual bool onPacketsReady(ROSEClient* client, std::queue<std::shared_ptr<Packet>>& packetQueue);
	void onNewClient(NetworkClient* nc) final;
	virtual void onNewROSEClient(ROSEClient* roseClient);
	virtual void onClientDisconnecting(NetworkClient* nc);

	virtual void onROSEClientDisconnecting(ROSEClient* client);
};

