#ifndef __POSTGRES_DATABASE__
#define __POSTGRES_DATABASE__

#pragma once

#include "Database.h"
#include "postgres_include\libpq-fe.h"

#pragma comment(lib, __FILE__"\\..\\libpq.lib")

class PostgresDatabase : public Database {
private:
	PGconn *connection;
	static constexpr char *URL_FORMAT = "user=%s password=%s host=%s port=%i dbname=%s";
	std::string connectionString;
public:
	PostgresDatabase(const DatabaseConnectionInformation& dbInformation) : Database(dbInformation) {
		char buf[0x200];
		sprintf_s(buf, URL_FORMAT, dbConnectionInformation.getUserName().c_str(), dbConnectionInformation.getPassword().c_str(),
			dbConnectionInformation.getHostName().c_str(), dbConnectionInformation.getPort(), dbConnectionInformation.getDatabaseName().c_str());
		connectionString = std::string(buf);

		connect();
	}

	virtual ~PostgresDatabase() {
	}

	virtual bool connect() {
		if (isConnected()) {
			return true;
		}
		connection = PQconnectdb(connectionString.c_str());
		return isConnected();
	}

	virtual void disconnect() {
		if (connection != nullptr) {
			PQfinish(connection);
		}
		connection = nullptr;
	}

	virtual std::shared_ptr<ResultSet> selectQuery(const char *query) {
		PGresult *result = PQexec(connection, query);
		if (result == nullptr) {
			return std::shared_ptr<ResultSet>();
		}
		int amount = PQntuples(result);
		int columns = PQnfields(result);
		if (amount == 0 || columns == 0) {
			return std::shared_ptr<ResultSet>();
		}
		std::shared_ptr<ResultSet> resultSet = std::shared_ptr<ResultSet>(new ResultSet(amount));

		std::vector<char*> row;
		row.reserve(columns);
		for (int i = 0; i<amount; i++) {
			for (int j = 0; j < columns; j++) {
				char *content = PQgetvalue(result, i, j);
				row.push_back(content);
			}
			ResultRow resultRow((const char**)row.data(), columns);
			resultSet->addToResult(resultRow);

			row.clear();
		}

		PQclear(result);
		return resultSet;
	}

	bool insertQuery(const char *query) {
		if (query == nullptr) {
			return false;
		}
		PGresult *result = PQexec(connection, query);
		if (!result) {
			return false;
		}
		int resultStatus = PQresultStatus(result);
		PQclear(result);
		return resultStatus == PGRES_COMMAND_OK;
	}

	bool updateQuery(const char *query) {
		return insertQuery(query);
	}

	int32_t insertQueryWithIdReturn(const char *query, const char* idColumnName) {
		char buf[0x300] = { 0x00 };
		std::string queryAsString = std::string(query);
		if (queryAsString.back() == ';') {
			queryAsString = queryAsString.substr(0, queryAsString.length() - 1);
		}
		sprintf_s(buf, "%s RETURNING %s", queryAsString.c_str(), idColumnName);
		PGresult *result = PQexec(connection, buf);
		if (!result) {
			return -1;
		}
		int amount = PQntuples(result);
		if (amount != 1) {
			return -1;
		}

		char *content = PQgetvalue(result, 0, 0);
		int32_t id = atoi(content);

		PQclear(result);

		return id;
	}

	__inline bool isConnected() const {
		return connection != nullptr && PQstatus(connection) == 0;
	}

	__inline const char* getLastError() const final {
		return PQerrorMessage(connection);
	}
};

#endif //