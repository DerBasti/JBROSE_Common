#include "PostgresDatabase.h"

const char *PostgresDatabase::URL_FORMAT = "user=%s password=%s host=%s port=%i dbname=%s";

PostgresDatabase::PostgresDatabase(const DatabaseConnectionInformation& dbInformation) : Database(dbInformation) {
	char buf[0x200];
	sprintf_s(buf, URL_FORMAT, dbConnectionInformation.getUserName().c_str(), dbConnectionInformation.getPassword().c_str(),
		dbConnectionInformation.getHostName().c_str(), dbConnectionInformation.getPort(), dbConnectionInformation.getDatabaseName().c_str());
	connectionString = std::string(buf);
	if (!connect()) {
		logger.logError("[DB-ERROR WHILE CONNECTING]: ", getLastError());
	}
}

PostgresDatabase::~PostgresDatabase() {
}

bool PostgresDatabase::connect() {
	if (isConnected()) {
		return true;
	}
	logger.logTrace("Connecting with connection string: ", connectionString.c_str());
	connection = PQconnectdb(connectionString.c_str());
	return isConnected();
}

void PostgresDatabase::disconnect() {
	if (connection != nullptr) {
		PQfinish(connection);
	}
	connection = nullptr;
}

std::shared_ptr<ResultSet> PostgresDatabase::selectQuery(const char *query) {
	logger.logTrace("Firing select-query at database: ", query);
	PGresult *result = PQexec(connection, query);
	if (result == nullptr) {
		return std::shared_ptr<ResultSet>(new ErrorResultSet(this->getLastError()));
	}
	int amount = PQntuples(result);
	int columns = PQnfields(result);
	if (amount == 0 || columns == 0) {
		char buffer[1024] = { 0x00 };
		sprintf_s(buffer, "No result found for query: %s", query);
		return std::shared_ptr<ResultSet>(new ErrorResultSet(buffer));
	}
	logger.logTrace("Found ", amount, " results.");
	std::shared_ptr<ResultSet> resultSet = std::shared_ptr<ResultSet>(new ResultSet(amount));

	std::vector<char*> row;
	row.reserve(columns);
	for (int i = 0; i < amount; i++) {
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

bool PostgresDatabase::insertQuery(const char *query) {
	if (query == nullptr) {
		return false;
	}
	logger.logTrace("Firing insert-query at database: ", query);
	PGresult *result = PQexec(connection, query);
	if (!result) {
		return false;
	}
	int resultStatus = PQresultStatus(result);
	logger.logTrace("Result of insert: ", resultStatus);
	PQclear(result);
	return resultStatus == PGRES_COMMAND_OK;
}

int64_t PostgresDatabase::insertQueryWithIdReturn(const char *query, const char* idColumnName) {
	char buf[0x300] = { 0x00 };
	std::string queryAsString = std::string(query);
	if (queryAsString.back() == ';') {
		queryAsString.pop_back();
	}
	sprintf_s(buf, "%s RETURNING %s", queryAsString.c_str(), idColumnName);
	logger.logTrace("Firing insert-query (with returning id) at database: ", buf);
	PGresult *result = PQexec(connection, buf);
	if (!result) {
		return -1;
	}
	int amount = PQntuples(result);
	if (amount != 1) {
		return -1;
	}

	char *content = PQgetvalue(result, 0, 0);
	int64_t id = _atoi64(content);
	logger.logTrace("Returning id is: ", id);

	PQclear(result);

	return id;
}

bool PostgresDatabase::deleteQuery(const char *query) {
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