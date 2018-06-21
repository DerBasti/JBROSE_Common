#include "FileReader.h"

FileReader::FileReader(const char *path) {
	fopen_s(&handle, path, "rb+");
	caret = 0;
}
FileReader::~FileReader() {
	if (handle != nullptr) {
		fclose(handle);
	}
	handle = nullptr;
}

std::shared_ptr<char> FileReader::readString(const uint32_t bytes) {
	std::shared_ptr<char> buffer = std::shared_ptr<char>(new char[bytes + 1], std::default_delete<char[]>());
	fread(buffer.get(), 1, bytes, handle);
	buffer.get()[bytes] = 0x00;
	caret += bytes;
	return buffer;
}