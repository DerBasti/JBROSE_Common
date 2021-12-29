#ifndef __ROSE_LOGGER__
#define __ROSE_LOGGER__

#include <inttypes.h>
#include <iostream>
#include <time.h>
#include <sstream> // stringstream
#include <iomanip> // put_time
#include <chrono>
#include <mutex>
#include <ctime>
#include <queue>

enum class LogLevelTextColorSchema : uint8_t {
	DEFAULT = 7,
	GREEN = 10,
	BLUE = 11,
	RED = 12,
	ORANGE = 14,
	WHITE = 15
};

class LogLevel {
private:
	const char* name;
	uint8_t importance;
	uint8_t colorScheme;
	constexpr LogLevel(const char* name, uint8_t urgency, const LogLevelTextColorSchema& colorSchema) : LogLevel(name, urgency, static_cast<uint8_t>(colorSchema)) {}
	constexpr LogLevel(const char* name, uint8_t urgency, const uint8_t colorSchema) : name(name), importance(urgency), colorScheme(colorSchema) {}
public:
	const static LogLevel* TRACE;
#ifdef DEBUG
#define _OTHER_DEBUG DEBUG
#undef DEBUG
#endif
	const static LogLevel* DEBUG;
	const static LogLevel* INFO;
	const static LogLevel* WARNING;
#ifdef ERROR
#define _OTHER_ERROR ERROR
#undef ERROR
#endif
	const static LogLevel* ERROR;
	const static LogLevel* DEFAULT_LOGLEVEL;

	virtual ~LogLevel() {

	}
	__inline bool operator==(const LogLevel& other) const {
		return getImportance() == other.getImportance();
	}
	__inline bool operator!=(const LogLevel& other) const {
		return !(this->operator==(other));
	}
	__inline bool operator<(const LogLevel& other) const {
		return getImportance() < other.getImportance();
	}
	__inline const char* getName() const {
		return name;
	}
	__inline uint8_t getImportance() const {
		return importance;
	}
	__inline uint8_t getColorScheme() const {
		return colorScheme;
	}
};

class ROSELogger {
private:
	const LogLevel* level;
	std::string loggerName;

	template<typename Arg>
	void logNext(Arg arg) const {
		std::wcout << arg;
	}

	template<>
	void logNext(std::string str) const {
		std::wcout << str.c_str();
	}
	template<>
	void logNext(std::wstring str) const {
		std::wcout << str.c_str();
	}

	template<typename NextType, typename... Arg>
	void logNext(NextType current, Arg... arg) const {
		logNext(current);
		logNext(arg...);
	}
	template<typename... Args>
	void log(const LogLevel* level, Args... args) const {
		if (*level < *(getLogLevel())) {
			return;
		}
		time_t currentTime;
		time(&currentTime);
		tm localTime;
		localtime_s(&localTime, &currentTime);
		std::stringstream stream;
		stream << std::put_time(&localTime, "%H:%M:%S");

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) % 1000;
		stream << '.' << std::setfill('0') << std::setw(3) << ms.count();

		std::wcout << "[" << stream.str().c_str() << "][";
		colorizeLogLevelOutput(level);
		std::wcout << "]";
		if (!loggerName.empty()) {
			std::wcout << "[" << loggerName.c_str() << "]";
		}
		std::wcout << " ";
		logNext(args...);
		std::wcout << "\n";
	}

protected:
	static void colorizeLogLevelOutput(const LogLevel* level);
public:
	ROSELogger();
	ROSELogger(const LogLevel* newLevel) : level(newLevel) {}
	virtual ~ROSELogger() {}  

	template<typename... Args>
	__inline void logTrace(Args... args) const {
#ifdef _DEBUG
		log(LogLevel::TRACE, args...);
#endif
	}

	template<typename... Args>
	__inline void logDebug(Args... args) const {
#ifdef _DEBUG
		log(LogLevel::DEBUG, args...);
#endif
	}

	template<typename... Args>
	__inline void logInfo(Args... args) const {
		log(LogLevel::INFO, args...);
	}	
	
	template<typename... Args>
	__inline void logWarn(Args... args) const {
		log(LogLevel::WARNING, args...);
	}	
		
	template<typename... Args>
	__inline void logError(Args... args) const {
		log(LogLevel::ERROR, args...);
	}

	__inline const LogLevel* getLogLevel() const {
		return level;
	}
	__inline void setLogLevel(const LogLevel* newLevel) {
		level = newLevel;
	}
	__inline std::string getLoggerName() const {
		return loggerName;
	}
	__inline void setLoggerName(const char* name) {
		this->loggerName = (name == nullptr ? std::string() : std::string(name));
	}
};

class ROSEThreadedLogger : public ROSELogger {
private:
	struct LogMessage {
		std::wstringstream output;
		const LogLevel* level;
		std::string loggerName;
	};
	static std::mutex inputMutex;
	static std::thread loggerThread;
	static std::queue<LogMessage> streamHolder;

	template<typename Arg>
	void logNext(std::wstringstream& wout, Arg arg) const {
		wout << arg;
	}

	template<>
	void logNext(std::wstringstream& wout, std::string str) const {
		wout << str.c_str();
	}
	template<>
	void logNext(std::wstringstream& wout, std::wstring str) const {
		wout << str.c_str();
	}

	template<typename NextType, typename... Arg>
	void logNext(std::wstringstream& wout, NextType current, Arg... arg) const {
		logNext(wout, current);
		logNext(wout, arg...);
	}
	template<typename... Args>
	void log(const LogLevel* level, Args... args) const {
		if (*level < *(getLogLevel())) {
			return;
		}
		std::wstringstream output;
		logNext(output, args...);
		output << "\n";
		std::lock_guard<std::mutex> lock(inputMutex);
		LogMessage message{ std::move(output), level, getLoggerName()};
		streamHolder.push(std::move(message));
	}

	static void printHeader(const LogMessage& message) {
		time_t currentTime;
		time(&currentTime);
		tm localTime;
		localtime_s(&localTime, &currentTime);
		std::stringstream stream;
		stream << std::put_time(&localTime, "%H:%M:%S");

		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) % 1000;
		stream << '.' << std::setfill('0') << std::setw(3) << ms.count();
		std::wcout << "[" << stream.str().c_str() << "][";
		colorizeLogLevelOutput(message.level);
		std::wcout << "]";
		if (!message.loggerName.empty()) {
			std::wcout << "[" << message.loggerName.c_str() << "]";
		}
		std::wcout << " ";
	}
public:
	ROSEThreadedLogger();
	ROSEThreadedLogger(const LogLevel* newLevel) : ROSELogger(newLevel) {}
	virtual ~ROSEThreadedLogger() {}

	static void init() {
		loggerThread = std::thread([&]() {
			streamHolder = std::queue<LogMessage>();
			while (true) {
				if (streamHolder.empty()) {
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
				else {
					std::queue<LogMessage> copy;
					{
						std::lock_guard<std::mutex> lock(inputMutex);
						copy = std::move(streamHolder);
						streamHolder = std::queue<LogMessage>();
					}
					while(!copy.empty()) {
						const LogMessage& messageEntry = copy.front();
						printHeader(messageEntry);
						std::wcout << messageEntry.output.str().c_str();
						copy.pop();
					}
				}
			}
		});
	}

	template<typename... Args>
	__inline void logTrace(Args... args) const {
#ifdef _DEBUG
		log(LogLevel::TRACE, args...);
#endif
	}

	template<typename... Args>
	__inline void logDebug(Args... args) const {
#ifdef _DEBUG
		log(LogLevel::DEBUG, args...);
#endif
	}

	template<typename... Args>
	__inline void logInfo(Args... args) const {
		log(LogLevel::INFO, args...);
	}

	template<typename... Args>
	__inline void logWarn(Args... args) const {
		log(LogLevel::WARNING, args...);
	}

	template<typename... Args>
	__inline void logError(Args... args) const {
		log(LogLevel::ERROR, args...);
	}
};

#ifdef _OTHER_ERROR
#define ERROR _OTHER_ERROR
#undef _OTHER_ERROR
#endif

#ifdef _OTHER_DEBUG
#define DEBUG _OTHER_DEBUG
#undef _OTHER_DEBUG
#endif

#endif //__ROSE_LOGGER__