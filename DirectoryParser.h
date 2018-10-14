#ifndef __ROSE_DIRECTORY_PARSER__
#define __ROSE_DIRECTORY_PARSER__
#pragma once

#include <windows.h>
#include <vector>
#include <string>

class DirectoryParser {
private:
	std::string path;
	std::string fileExtension;
	std::string fullSearchExpression;
	std::vector<std::string> files;
	void readFilesFromDirectory() {
		WIN32_FIND_DATAA fileData = { 0x00 };
		fullSearchExpression = (path + fileExtension);
		HANDLE handle = FindFirstFileA(fullSearchExpression.c_str(), &fileData);
		if (handle != INVALID_HANDLE_VALUE) {
			do {
				std::string fileName = path + std::string(fileData.cFileName);
				files.push_back(fileName);
			} while (FindNextFileA(handle, &fileData));
			FindClose(handle);
		}
	}
	void setFileExtensionWithoutUpdate(const char* extension) {
		fileExtension = std::string(extension);
		if (!this->fileExtension.empty() && this->fileExtension.at(0x00) != '.') {
			this->fileExtension = std::string(".") + this->fileExtension;
		}
		this->fileExtension = std::string("*") + this->fileExtension;
	}
public:
	DirectoryParser(const char *filePath) : DirectoryParser(filePath, "") {

	}
	DirectoryParser(const char *filePath, const char *fileExtension) {
		path = std::string(filePath);
		this->fileExtension = fileExtension;
		if (path.at(path.length() - 1) != '\\') {
			path = path + std::string("\\");
		}
		setFileExtensionWithoutUpdate(fileExtension);
		readFilesFromDirectory();
	}

	virtual ~DirectoryParser() {

	}

	bool isDirectoryExistent() const {
		WIN32_FIND_DATAA fileData = { 0x00 };
		HANDLE handle = FindFirstFileA((path + std::string("*")).c_str(), &fileData);
		bool success = handle != INVALID_HANDLE_VALUE && ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
		if (success) {
			FindClose(handle);
		}
		return success;
	}

	static bool isFileExistent(const char* path) {
		WIN32_FIND_DATAA fileData = { 0x00 };
		HANDLE handle = FindFirstFileA(path, &fileData);
		bool success = handle != INVALID_HANDLE_VALUE && ((fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
		if (success) {
			FindClose(handle);
		}
		return success;
	}

	void setFileExtension(const char* extension) {
		setFileExtensionWithoutUpdate(extension);
		updateEntries();
	}

	__inline void updateEntries() {
		files.clear();
		readFilesFromDirectory();
	}
	__inline const std::vector<std::string>& getFiles() const {
		return files;
	}
};

#endif //__ROSE_DIRECTORY_PARSER__