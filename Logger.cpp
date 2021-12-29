#include "Logger.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef ERROR
#undef ERROR
#endif

const LogLevel* LogLevel::TRACE = new LogLevel("TRACE", 0, LogLevelTextColorSchema::GREEN);
const LogLevel* LogLevel::DEBUG = new LogLevel("DEBUG", 1, LogLevelTextColorSchema::BLUE);
const LogLevel* LogLevel::INFO = new LogLevel("INFO ", 2, LogLevelTextColorSchema::WHITE);
const LogLevel* LogLevel::WARNING = new LogLevel("WARN ", 3, LogLevelTextColorSchema::ORANGE);
const LogLevel* LogLevel::ERROR = new LogLevel("ERROR", 4, LogLevelTextColorSchema::RED);

#ifdef _DEBUG
const LogLevel* LogLevel::DEFAULT_LOGLEVEL = LogLevel::DEBUG;
#else
const LogLevel* LogLevel::DEFAULT_LOGLEVEL = LogLevel::INFO;
#endif

ROSELogger::ROSELogger() : ROSELogger(LogLevel::DEFAULT_LOGLEVEL) {

}

void ROSELogger::colorizeLogLevelOutput(const LogLevel* level) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, level->getColorScheme());
	std::wcout << level->getName();
	SetConsoleTextAttribute(hConsole, static_cast<uint8_t>(LogLevelTextColorSchema::DEFAULT));
}

ROSEThreadedLogger::ROSEThreadedLogger() : ROSEThreadedLogger(LogLevel::DEFAULT_LOGLEVEL) {

}

const char *colors[] = {
	"\x1b[0m",
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	nullptr,
	"\x1b[37m", //WHITE
	nullptr,
	nullptr,
	"\x1b[32m", //GREEN
	"\x1b[34m", //BLUE
	"\x1b[31m", //RED
	nullptr,
	"\x1b[33m",
	"\x1b[1m"
};

std::mutex ROSEThreadedLogger::inputMutex;
std::thread ROSEThreadedLogger::loggerThread;
std::queue<ROSEThreadedLogger::LogMessage> ROSEThreadedLogger::streamHolder;