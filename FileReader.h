#ifndef __ROSE_FILE_READER__
#define __ROSE_FILE_READER__
#pragma once

#include <iostream>
#include <memory>
#include <type_traits>

class FileReader {
private:
	FILE * handle;
	uint64_t caret;
	uint64_t size;
	template<class _T, class = typename std::enable_if<std::is_integral<_T>::value || std::is_floating_point<_T>::value>::type>
	_T read() {
		_T value;
		uint8_t typeSize = sizeof(_T);
		fread(&value, typeSize, 1, handle);
		caret += typeSize;
		return value;
	}
public:
	FileReader(const char *path);
	virtual ~FileReader();

	__inline void skipBytes(const uint64_t amountOfBytes) {
		_fseeki64(handle, amountOfBytes, SEEK_CUR);
		caret += amountOfBytes;
	}

	__inline void setCaret(const uint64_t caretPosition) {
		_fseeki64(handle, caretPosition, SEEK_SET);
		caret = caretPosition;
	}

	__inline uint64_t getCaret() const {
		return caret;
	}

	__inline uint32_t readUInt() {
		return read<uint32_t>();
	}
	__inline uint16_t readUShort() {
		return read<uint16_t>();
	}
	__inline uint8_t readByte() {
		return read<uint8_t>();
	}
	__inline float readFloat() {
		return read<float>();
	}
	std::shared_ptr<char> readString(const uint32_t bytes);
};

#endif //__ROSE_FILE_READER__
