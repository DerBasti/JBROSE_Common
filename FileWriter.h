#ifndef __ROSE_FILE_WRITER__
#define __ROSE_FILE_WRITER__

#include <stdio.h>
#include <string>
#include <ostream>

class FileWriter {
private:
	FILE* file;
	template<typename Arg>
	void logNext(std::wstringstream& stream, Arg arg) const {
		stream << arg;
	}

	template<>
	void logNext(std::wstringstream& stream, std::string str) const {
		stream << str.c_str();
	}

	template<>
	void logNext(std::wstringstream& stream, std::wstring str) const {
		stream << str.c_str();
	}

	template<>
	void logNext(std::wstringstream& stream, uint64_t num) const {
		char buffer[0x60] = { 0x00 };
		sprintf_s(buffer, "%llu", num);
		stream << buffer;
	}

	template<typename NextType, typename... Arg>
	void logNext(std::wstringstream& stream, NextType current, Arg... arg) const {
		logNext(stream, current);
		logNext(stream, arg...);
	}

	template<typename... Args>
	void log(Args... args) const {
		std::wstringstream stream;
		logNext(stream, args...);
		fputws(stream.str().c_str(), file);
	}
public:
	FileWriter(const char* path) {
		fopen_s(&file, path, "wb+");
	}
	virtual ~FileWriter() {
		close();
		file = nullptr;
	}
	template<typename... Args>
	void writeToFile(Args... args) {
		log(args...);
	}
	void close() {
		if (file) {
			fclose(file);
		}
	}
};


#endif //__ROSE_FILE_WRITER__