#include "FileReader.h"

FileReader::FileReader(const char *path) {
	int filePathLength = path == nullptr ? 0 : static_cast<int>(strlen(path));
	char *pathHolder = new char[filePathLength + 1];
	strncpy_s(pathHolder, filePathLength+1, (path == nullptr ? "" : path), filePathLength);
	pathHolder[filePathLength] = 0x00;
	this->filePath = std::shared_ptr<char>(std::move(pathHolder), std::default_delete<char[]>());
}

FileReader::~FileReader() {
}

FileInputReader::FileInputReader(const char *path) : FileReader(path) {
	fopen_s(&fileHandle, path, "rb+");
	if (fileHandle) {
		fseek(fileHandle, 0x00, SEEK_END);
		setFileSize(_ftelli64(fileHandle));
		rewind(fileHandle);
	}
}

FileInputReader::~FileInputReader() {
	if (fileHandle != nullptr) {
		fclose(fileHandle);
	}
	fileHandle = nullptr;
}

char* FileInputReader::readString(const uint64_t bytes) {
	char* buffer = new char[bytes + 1];
	fread(buffer, 1, bytes, fileHandle);
	buffer[bytes] = 0x00;
	addToCaret(bytes);
	return buffer;
}

char* FileInputReader::readString() {
	std::string result = std::string();
	char currentChar = 0x00;
	while ((currentChar = static_cast<char>(readByte())) != 0x00 && getCaret() < getFileSize()) {
		result += currentChar;
	}
	char *dataHolder = new char[result.length() + 1];
	strncpy_s(dataHolder, result.length() + 1, result.c_str(), result.length());
	dataHolder[result.length()] = 0x00;
	return dataHolder;
}