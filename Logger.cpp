#include "Logger.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#ifdef ERROR
#undef ERROR
#endif

const uint8_t LogLevel::DEFAULT_COLOR_SCHEME = 7;

const LogLevel* LogLevel::TRACE = new LogLevel("TRACE", 0, 10);
const LogLevel* LogLevel::DEBUG = new LogLevel("DEBUG", 1, 11);
const LogLevel* LogLevel::INFO = new LogLevel("INFO ", 2, 15);
const LogLevel* LogLevel::WARNING = new LogLevel("WARN ", 3, 14);
const LogLevel* LogLevel::ERROR = new LogLevel("ERROR", 4, 12);
#ifdef _DEBUG
const LogLevel* LogLevel::DEFAULT_LOGLEVEL = LogLevel::DEBUG;
#else
const LogLevel* LogLevel::DEFAULT_LOGLEVEL = LogLevel::INFO;
#endif

ROSELogger::ROSELogger() : ROSELogger(LogLevel::DEFAULT_LOGLEVEL) {

}

void ROSELogger::colorizeLogLevelOutput(const LogLevel* level) const {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, level->getColorScheme());
	std::cout << level->getName();
	SetConsoleTextAttribute(hConsole, LogLevel::DEFAULT_COLOR_SCHEME);
}