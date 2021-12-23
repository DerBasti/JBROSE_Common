#ifndef __POSTGRES_DATABASE__
#define __POSTGRES_DATABASE__

#pragma once

#include "Database.h"
#include "postgres_include\libpq-fe.h"

#pragma comment(lib, __FILE__"\\..\\libpq.lib")

class PostgresDatabase : public Database {
private:
	PGconn *connection;
	static const char *URL_FORMAT;
	std::string connectionString;
public:
	PostgresDatabase(const DatabaseConnectionInformation& dbInformation);
	virtual ~PostgresDatabase();
	virtual bool connect();
	virtual void disconnect();
	virtual std::shared_ptr<ResultSet> selectQuery(const char *query);
	bool insertQuery(const char *query);

	__inline bool updateQuery(const char *query) {
		return insertQuery(query);
	}

	virtual bool deleteQuery(const char *query);

	int64_t insertQueryWithIdReturn(const char *query, const char* idColumnName);

	__inline bool isConnected() const {
		return connection != nullptr && PQstatus(connection) == 0;
	}

	__inline const char* getLastError() const final {
		return PQerrorMessage(connection);
	}
};

#endif //