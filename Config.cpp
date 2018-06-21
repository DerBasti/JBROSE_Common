#include "Config.h"
#include <algorithm>

Config::Config() {

}

Config::Config(const char* configFilePath) {
	FILE* fh = nullptr;
	if (fopen_s(&fh, configFilePath, "rb") == 0) {
		parseConfigFile(fh);
		fclose(fh);
	}
	fh = nullptr;
}

Config::~Config() {

}

void Config::parseConfigFile(FILE *file) {
	std::vector<std::string> lines = parseConfigFileLines(file);
	std::for_each(lines.begin(), lines.end(), [&](std::string line) {
		int equalIndex = line.find('=');
		if (equalIndex != -1 && line.find('=', equalIndex + 1) == -1) {
			std::string key = line.substr(0, equalIndex);
			std::string value = line.substr(equalIndex + 1);
			keyValueStorage.insert(std::make_pair(key, value));
		}
	});
}

std::vector<std::string> Config::parseConfigFileLines(FILE* file) {
	char currentChar = 0x00;
	std::string currentLine;
	std::vector<std::string> lines;
	while (fread(&currentChar, 1, 1, file) > 0 && feof(file) == 0) {
		if (currentChar == '\r') {
			continue;
		}
		if (currentChar == '\n') {
			if (currentLine.length() > 0 && currentLine.at(0) != '#') {
				lines.push_back(currentLine);
			}
			currentLine.clear();
		}
		else {
			currentLine += currentChar;
		}
	}
	lines.push_back(currentLine);
	return lines;
}
