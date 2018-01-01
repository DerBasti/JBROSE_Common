#pragma once
#include <memory>
class NetworkMessageFragment
{
private:
	uint16_t length;
	std::shared_ptr<char> message;
public:
	NetworkMessageFragment() {
		message = std::shared_ptr<char>(new char[0x400], std::default_delete<char[]>());
	}
	~NetworkMessageFragment() {
	}

	operator char*() const {
		return message.get();
	}

	__inline const char *getMessage() const {
		return (*this);
	}

	__inline uint16_t getLength() const {
		return length;
	}
	__inline void setLength(const uint16_t len) {
		length = len;
	}
};

