#pragma once
#include "NetworkServer.h"
#include "ROSEClient.h"
#include "EncryptionHandler.h"
#include "PacketFactory.h"
#include "Database.h"
#include <memory>
#include <unordered_map>

class ROSEServer : public NetworkServer
{
private:
	static ROSEServer* instance;
	std::unordered_map < NetworkClient*, std::shared_ptr<ROSEClient> > clientList;
	std::function<std::shared_ptr<PacketFactory>()> packetFactoryCreatorFunction;

	std::shared_ptr<ROSEClient> findROSEClientByInterface(NetworkClient* client);
	void onPrepareDataIncoming(NetworkClient* nc);
	void onDataReceived(NetworkClient* nc, const NetworkMessageFragment& dataHolder) final;
	void onNewClient(NetworkClient* nc) final;
	void onClientDisconnecting(NetworkClient* nc);
protected:
	static std::shared_ptr<CryptTable> ENCRYPTION_TABLE;
	Database *database;

	virtual void loadEncryption();

	virtual bool onPacketsReady(std::shared_ptr<ROSEClient>& client, std::queue<std::shared_ptr<Packet>>& packetQueue);
	virtual void onNewROSEClient(std::shared_ptr<ROSEClient>& roseClient);
	virtual void onROSEClientDisconnecting(std::shared_ptr<ROSEClient>& client);
	
	__inline void setPacketFactoryCreatorFunction(std::function<std::shared_ptr<PacketFactory>()> factoryFunction) {
		packetFactoryCreatorFunction = factoryFunction;
	}
public:
	ROSEServer(uint16_t port);
	~ROSEServer();

	__inline static const CryptTable* getEncryptionTable() {
		return ENCRYPTION_TABLE.get();
	}
	
	template<class _T, class = typename std::enable_if<std::is_base_of<ROSEServer, _T>::value>::type>
	__inline static _T* getInstance() {
		return (_T*)instance;
	}

	__inline Database* getDatabase() {
		return database;
	}
};

