#ifndef __MYSQL_DATABASE__
#define __MYSQL_DATABASE__

#pragma once

#include "Database.h"
#include "mysql_include\mysql.h"

#pragma comment(lib, __FILE__ "\\..\\libmysql.lib")


class MySQLDatabase : public Database {
private:
	MYSQL handle;

	void extractResult(MYSQL_RES *res, std::shared_ptr<ResultSet>& resultSet) {
		MYSQL_ROW row;
		while ((row = mysql_fetch_row(res)) != nullptr) {
			ResultRow resultRow = ResultRow((const char**)row, mysql_field_count(&handle));
			resultRow.setColumnNames(resultSet->getColumnNames());
			resultSet->addToResult(resultRow);
		}
	}

	void extractColumnData(MYSQL_RES* mysqlResult, std::shared_ptr<ResultSet>& resultSet) {
		uint16_t fieldAmount = static_cast<uint16_t>(mysql_num_fields(mysqlResult));
		MYSQL_FIELD* fields = mysql_fetch_fields(mysqlResult);
		std::vector<std::string> fieldNames;
		fieldNames.reserve(fieldAmount);
		for (uint32_t i = 0; i < fieldAmount; i++) {
			fieldNames.push_back(std::string(fields[i].name));
		}
		resultSet->setColumnNames(fieldNames);
	}
protected:
	virtual std::vector<std::string> getColumnNames() {
		return std::vector<std::string>();
	}
public:
	MySQLDatabase(const DatabaseConnectionInformation& dbInfo) : Database(dbInfo) {
		connect();
	}
	virtual ~MySQLDatabase() {
	}

	bool connect() {
		mysql_init(&handle);
		bool success = mysql_real_connect(&handle, dbConnectionInformation.getHostName().c_str(),
			dbConnectionInformation.getUserName().c_str(), dbConnectionInformation.getPassword().c_str(),
			dbConnectionInformation.getDatabaseName().c_str(), dbConnectionInformation.getPort(), nullptr, 0) != nullptr;
		if (!success) {
			std::cout << "Couldnt connect: " << mysql_error(&handle);
		}
		else {
			mysql_query(&handle, "USE rose_online;");
		}
		return success;
	}
	void disconnect() {
		mysql_close(&handle);
	}

	std::shared_ptr<ResultSet> selectQuery(const char *query) {
		std::shared_ptr<ResultSet> result;
		if (mysql_query(&handle, query) == 0) {
			MYSQL_RES *res = mysql_store_result(&handle);

			uint32_t numRows = static_cast<uint32_t>(mysql_num_rows(res));
			result = std::shared_ptr<ResultSet>(new ResultSet(numRows));
			extractColumnData(res, result);
			extractResult(res, result);

			mysql_free_result(res);
		}
		return result;
	}

	bool insertQuery(const char *query) {
		if (query == nullptr) {
			return false;
		}
		const size_t insertLen = 7;
		const size_t queryLen = strlen(query);
		if (queryLen <= insertLen) {
			return false;
		}
		const char *insert = "INSERT";
		bool validQueryFlag = false;
		while (*insert != '\0' && (validQueryFlag = (*insert == *query))) {
			query++;
			insert++;
		}
		return validQueryFlag && mysql_real_query(&handle, query, queryLen) == 0;
	}

	__inline uint32_t getErrorCode() const {
		return mysql_errno(const_cast<MYSQL*>(&handle));
	}

	__inline const char* getLastError() const final {
		return mysql_error(const_cast<MYSQL*>(&handle));
	}
};

#endif //__MYSQL_DATABASE__