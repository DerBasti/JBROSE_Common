#pragma once
#include <memory>
#include <string>
#include <functional>
#include <vector>

template<class T, class = typename std::enable_if<std::is_pointer<T>::value == false>::type>
class Supplier {
private:
	std::function<T()> supplyingFunction;
public:
	Supplier(std::function<T()> supply) {
		supplyingFunction = supply;
	}
	Supplier& operator=(const std::function<T()>& other) {
		supplyingFunction = other;
		return (*this);
	}
	T operator()() const {
		return supplyingFunction();
	}
};

class Packet;

class SendablePacket {
private:
	std::shared_ptr<unsigned char> sharedBuffer;
	uint16_t caret;
public:
	SendablePacket(const Packet& packet);

	template<class T>
	void addData(T value) {
		uint8_t *buffer = sharedBuffer.get();
		uint16_t sizeOfT = static_cast<uint16_t>(sizeof(T));
		memcpy(&buffer[caret], &value, sizeOfT);
		caret += sizeOfT;
	}

	void addString(const char *str) {
		uint8_t *buffer = sharedBuffer.get();
		unsigned short stringLength = str != nullptr ? static_cast<unsigned short>(strlen(str)) : 0;
		memcpy(&buffer[caret], str, stringLength);
		caret += stringLength;

		addData<uint8_t>(0x00); //Null-Terminator
	}

	__inline void updatePacketSize() const {
		uint16_t* packet = (uint16_t*)sharedBuffer.get();
		*packet = caret;
	}

	__inline uint16_t getCurrentSize() const {
		return caret;
	}

	std::shared_ptr<unsigned char> toSendable() const {
		return sharedBuffer;
	}
};

class Packet
{
private:
	uint16_t length;
	uint16_t commandId;
	uint16_t hash;
	std::shared_ptr<char> rawData;
protected:
	Packet(uint16_t command, uint16_t len) {
		commandId = command;
		length = len;
	}
	__inline void setLength(uint16_t newLength) {
		length = newLength;
	}
	void onNewString(const char *oldStr, const char *newStr) {
		size_t oldLen = oldStr != nullptr ? strlen(oldStr)+1 : 0;
		size_t newLen = newStr != nullptr ? strlen(newStr)+1 : 0;

		int16_t differenceToPreviousState = static_cast<int16_t>(oldLen - newLen);
		setLength(static_cast<uint16_t>(length - differenceToPreviousState));
	}
public:
	const static uint16_t DEFAULT_MINIMUM_SIZE = 6;
	Packet();
	Packet(const char* data);
	~Packet();

	__inline bool isRawDataSet() const {
		return rawData.operator bool();
	}
	const char* getRawData() const {
		return rawData.get();
	}

	virtual std::string toPrintable() const {
		char buf[0x100] = { 0x00 };
		sprintf_s(buf, "[0x%x - %i bytes]", commandId, length);
		return std::string(buf);
	}

	__inline uint16_t getLength() const {
		return length;
	}
	__inline uint16_t getCommandId() const {
		return commandId;
	}
}; 

class ResponsePacket : public Packet {
protected:
	void appendHeaderToSendable(SendablePacket& sendable) const {
		sendable.addData<uint16_t>(getLength()); //Length
		sendable.addData(getCommandId());
		sendable.addData<uint16_t>(0x00);
	}
	virtual void appendContentToSendable(SendablePacket& sendable) const {

	}
public:
	ResponsePacket(const uint16_t commandId, const uint16_t len) : Packet(commandId, len) {

	}
	SendablePacket toSendable() const;
};