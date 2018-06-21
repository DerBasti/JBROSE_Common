#ifndef __DATABASE_INTERFACE__
#define __DATABASE_INTERFACE__

#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <functional>
#include <map>

class DatabaseConnectionInformation {
private:
	std::string hostName;
	uint16_t port;
	std::string userName;
	std::string password;
	std::string databaseName;
public:
	DatabaseConnectionInformation() {
		hostName = std::string("127.0.0.1");
		port = 0;
	}
	virtual ~DatabaseConnectionInformation() {

	}

	__inline std::string getHostName() const {
		return hostName;
	}
	__inline void setHostName(const std::string& host) {
		hostName = host;
	}
	__inline uint16_t getPort() const {
		return port;
	}
	__inline void setPort(const uint16_t& port) {
		this->port = port;
	}
	__inline std::string getUserName() const {
		return userName;
	}
	__inline void setUserName(const std::string& username) {
		userName = username;
	}
	__inline std::string getPassword() const {
		return this->password;
	}
	__inline void setPassword(const std::string& password) {
		this->password = password;
	}
	__inline std::string getDatabaseName() const {
		return databaseName;
	}
	__inline void setDatabaseName(const std::string& dbName) {
		databaseName = dbName;
	}
};


class ResultRow {
private:
	std::shared_ptr<const char*> columnNames;
	uint16_t columnCount;
	std::vector<std::string> dataHolder;
public:
	ResultRow() {
		columnCount = 0;
	}
	ResultRow(const char **data, const uint16_t columnAmount) {
		columnCount = columnAmount;
		dataHolder.reserve(columnCount);
		for (uint16_t i = 0; i < columnCount; i++, data++) {
			std::string columnData = (*data) == nullptr ? std::string() : std::string(*data);
			dataHolder.push_back(columnData);
		}
	}

	std::string getColumnData(const uint16_t columnId) const {
		if (columnId >= columnCount) {
			return std::string();
		}
		return dataHolder.at(columnId);
	}

	__inline int32_t getColumnDataAsInt(const uint16_t columnId) const {
		return atoi(getColumnData(columnId).c_str());
	}

	__inline std::shared_ptr<const char*> getColumnNames() const {
		return columnNames;
	}

	__inline void setColumnNames(std::shared_ptr<const char*>& columns) {
		columnNames = columns;
	}

	__inline uint16_t getColumnCount() const {
		return columnCount;
	}
};

class ResultSet {
private:
	ResultRow* resultRows;
	std::shared_ptr<const char*> columnNames;
	uint16_t columnAmount;
	uint32_t rowAmount;
	uint32_t rowIndexForAdding;
public:
	ResultSet(uint32_t rowAmount) {
		this->rowAmount = rowAmount;
		resultRows = new ResultRow[rowAmount + 1];
	}

	virtual ~ResultSet() {
		rowAmount = 0;

		delete[] resultRows;
		resultRows = nullptr;
	}

	virtual void addToResult(const ResultRow& row) {
		if (rowIndexForAdding < rowAmount) {
			resultRows[rowIndexForAdding] = row;
			rowIndexForAdding++;
		}
	}

	void setColumnNames(std::vector<std::string>& columnNames) {
		columnAmount = static_cast<uint16_t>(columnNames.size());
		const char** columns = new const char*[columnAmount];
		for (uint32_t i = 0; i < columnAmount; i++) {
			std::string& col = columnNames.at(i);
			char *ptr = new char[col.length() + 1];
			strncpy_s(ptr, col.length()+1, col.c_str(), col.length());
			ptr[col.length()] = 0x00;
			columns[i] = ptr;
		}
		this->columnNames = std::shared_ptr<const char*>(columns, [&](const char** ptr) {
			for (unsigned int i = 0; i < columnAmount; i++) {
				delete[] ptr[i];
				ptr[i] = nullptr;
			}
			delete[] ptr;
			ptr = nullptr;
		});
	}

	__inline std::shared_ptr<const char*> getColumnNames() const {
		return columnNames;
	}

	__inline uint16_t getColumnAmount() const {
		return columnAmount;
	}

	__inline ResultRow* getRow(const uint32_t row) const {
		return &resultRows[row];
	}

	__inline uint32_t getResultAmount() const {
		return rowAmount;
	}
};

class Database {
protected:
	DatabaseConnectionInformation dbConnectionInformation;
public:
	Database(const DatabaseConnectionInformation& dbInformation) {
		dbConnectionInformation = dbInformation;
		connect();
	}
	virtual ~Database() {
		disconnect();
	}
	virtual bool connect() {
		return false;
	}
	virtual void disconnect() {

	}
	virtual bool isConnected() const {
		return false;
	}
	std::shared_ptr<ResultSet> selectQuery(const std::string& query) {
		return selectQuery(query.c_str());
	}
	virtual std::shared_ptr<ResultSet> selectQuery(const char *query) {
		return std::shared_ptr<ResultSet>();
	}
	 bool deleteQuery(const std::string& query) {
		return deleteQuery(query.c_str());
	}
	virtual bool deleteQuery(const char *query) {
		return false;
	}
	bool insertQuery(const std::string& query) {
		return insertQuery(query.c_str());
	}
	virtual bool insertQuery(const char *query) {
		return false;
	}

	bool updateQuery(const std::string& query) {
		return updateQuery(query.c_str());
	}

	virtual bool updateQuery(const char *query) {
		return false;
	}

	virtual int32_t insertQueryWithIdReturn(const char *query, const char* idColumnName) {
		return -1;
	}
	__inline virtual const char* getLastError() const {
		return "";
	}
};

#endif //__DATABASE_INTERFACE__