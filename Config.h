#ifndef __CONFIG_READER__
#define __CONFIG_READER__

#pragma once

#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <type_traits>

class Config {
private:
	std::map<std::string, std::string> keyValueStorage;

	void parseConfigFile(FILE *file);
	std::vector<std::string> parseConfigFileLines(FILE* file);

public:
	Config();
	Config(const char* configFilePath);
	virtual ~Config();

	__inline void addValue(const char *key, const char* value) {
		addValue(std::string(key), std::string(value));
	}

	__inline void addValue(const std::string& key, const std::string& value) {
		keyValueStorage.insert(std::make_pair(key, value));
	}

	__inline std::string getValue(const char *key) const {
		return keyValueStorage.at(key);
	}

	template<class _T = int32_t, class = std::enable_if<std::is_integral<_T>::value>::type>
	__inline _T getValueAsInt(const char* key) const {
		return static_cast<_T>(atoll(getValue(key).c_str()));
	}

	template<class _T = float, class = std::enable_if<std::is_floating_point<_T>::value>::type>
	__inline _T getValueAsFloatingPoint(const char* key) const {
		return static_cast<_T>(atof(getValue(key).c_str()));
	}

	__inline bool isKeyExisting(const char* key) const {
		return keyValueStorage.find(key) != keyValueStorage.end();
	}
};

#endif //__CONFIG_READER__