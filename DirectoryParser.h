#ifndef __ROSE_DIRECTORY_PARSER__
#define __ROSE_DIRECTORY_PARSER__
#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <algorithm> 

class Directory {
private:
	std::string path;
	std::vector<std::shared_ptr<Directory>> subDirectories;
	std::vector<std::string> files;
public:
	Directory(const std::string& path) {
		this->path = path;
	}
	virtual ~Directory() {

	}
};

class DirectoryParser {
private:
	std::string searchPath;
	std::string fileExtension;
	std::vector<std::string> listOfFiles;
	std::vector<std::string> folders;
	bool searchPathExistent;

protected:
	virtual std::vector<std::string> parseDirectoryRecursively(const std::string& path) const {
		return std::vector<std::string>();
	}
	virtual std::vector<std::string> parseDirectory(const std::string& path) const {
		return std::vector<std::string>();
	}
	virtual void validateSearchPath() {

	}
	bool isFileExtensionMatching(const std::string& fileName) const {
		int64_t position = fileName.find_last_of('.');
		bool found = false;
		if (position >= 0) {
			const char* fileNamePtr = &(fileName.data()[position]);
			return _stricmp(fileNamePtr, fileExtension.data()) == 0;
		}
		return false;
	}
	__inline bool isFileExtensionSet() const {
		return fileExtension.length()>=2;
	}
	void setPathExistent(bool validFlag) {
		searchPathExistent = validFlag;
	}
public:
	DirectoryParser(const char *searchPath) : DirectoryParser(searchPath, nullptr) {
	}
	DirectoryParser(const char* searchPath, const char* fileExtension) {
		setSearchPath(searchPath);
		setFileExtensionToSearchFor(fileExtension);
	}
	virtual ~DirectoryParser() {

	}
	void loadFileList() {
		std::function<std::vector<std::string>(const std::string&)> searchMethod;
		if (isRecursiveSearchEnabled()) {
			searchMethod = std::bind(&DirectoryParser::parseDirectoryRecursively, this, std::placeholders::_1);
		}
		else {
			searchMethod = std::bind(&DirectoryParser::parseDirectory, this, std::placeholders::_1);
		}
		listOfFiles = std::move(searchMethod(getSearchPath()));
	}
	__inline static bool isFileExistent(const char* path) {
#ifdef _WIN32
		WIN32_FIND_DATAA ffd = { 0x00 };
		HANDLE handle = FindFirstFileA(path, &ffd);
		bool result = handle != INVALID_HANDLE_VALUE && (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
		FindClose(handle);
		return result;
#else
		return false;
#endif
	}
	__inline bool isRecursiveSearchEnabled() const {
		return true;
	}
	__inline const std::vector<std::string>& getFileList() const {
		return listOfFiles;
	}
	__inline std::string getSearchPath() const {
		return searchPath;
	}
	__inline void setSearchPath(const char* searchPath) {
		setSearchPath(searchPath == nullptr ? std::string("") : std::string(searchPath));
	}
	__inline void setSearchPath(const std::string& searchPath) {
		this->searchPath = searchPath;
	}
	__inline std::string getFileExtensionToSearchFor() const {
		return fileExtension;
	}
	__inline void setFileExtensionToSearchFor(const char* extension) {
		setFileExtensionToSearchFor(extension == nullptr ? std::string("") : std::string(extension));
	}
	__inline void setFileExtensionToSearchFor(const std::string& extension) {
		std::string extensionToSet = extension;
		if (extension.length() > 0 && extension.at(0) != '.') {
			extensionToSet = std::string(".") + extension;
		}
		this->fileExtension = extensionToSet;
	}
	__inline bool isPathExistent() const {
		return searchPathExistent;
	}
};

class WindowsDirectoryParser : public DirectoryParser {
protected:
	virtual std::vector<std::string> parseDirectoryRecursively(const std::string& path) const {
		WIN32_FIND_DATAA ffd = { 0x00 };
		std::vector<std::string> result;
		HANDLE handle = FindFirstFileA((path + std::string("\\*")).c_str(), &ffd);
		do {
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				std::string subPath = path + std::string("\\") + std::string(ffd.cFileName);
				if (subPath.find_last_of(".") != subPath.length() - 1) {
					std::vector<std::string> subfiles = parseDirectoryRecursively(subPath);
					result.insert(result.end(), subfiles.begin(), subfiles.end());
				}
			}
			else {
				std::string fileName = std::string(ffd.cFileName);
				if (!isFileExtensionSet() || isFileExtensionMatching(fileName)) {
					result.push_back(path + std::string("\\") + std::string(ffd.cFileName));
				}
			}
		} while (FindNextFileA(handle, &ffd) != 0);
		FindClose(handle);
		return result;
	}
	virtual std::vector<std::string> parseDirectory(const std::string& path) const {
		return std::vector<std::string>();
	}
	virtual void validateSearchPath() {
		WIN32_FIND_DATAA ffd = { 0x00 };
		std::vector<std::string> result;
		HANDLE handle = FindFirstFileA((getSearchPath() + std::string("\\*")).c_str(), &ffd);
		setPathExistent(handle != INVALID_HANDLE_VALUE);
		FindClose(handle);
	}
public:
	WindowsDirectoryParser(const char *searchPath) : WindowsDirectoryParser(searchPath, nullptr) {
	}
	WindowsDirectoryParser(const char* searchPath, const char* fileExtension) : DirectoryParser(searchPath, fileExtension) {
		validateSearchPath();
	}
	virtual ~WindowsDirectoryParser() {

	}
};

#endif //__ROSE_DIRECTORY_PARSER__