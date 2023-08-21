#pragma once

#include <iostream>
#include <string>
#include <mariadb/conncpp.hpp>
#include "../util/StringUtil.h"

using namespace std;
using namespace sql;

inline void setParameters(const vector<string> &parameters, string &sql) {
	for (unsigned i = 0; i < parameters.size(); i++) {
		string param = to_string(i) + "?";
		replace(sql, param, parameters[i]);
	}
}

struct DatabaseManager {

	inline static string CONNECTION_URL = "";
	inline static string USER = "";
	inline static string PASSWD = "";

	Connection *connection = NULL;

public:

	~DatabaseManager() {
		closeConnection();
	}

	void executeUpdate(string &sql, const vector<string> &parameters) {
		PreparedStatement *stmnt = prepareStatement(parameters, sql);
		stmnt->executeUpdate();
		delete stmnt;
	}

	ResultSet* executeQuery(string &sql, const vector<string> &parameters) {
		PreparedStatement *stmnt = prepareStatement(parameters, sql);
		ResultSet *result = stmnt->executeQuery();
		delete stmnt;
		return result;
	}

	void openConnection() {
		if (this->connection == NULL || this->connection->isClosed()) {
			if (this->connection != NULL && !this->connection->isClosed()) {
				this->connection->close();
				delete this->connection;
			}
			Driver *driver = mariadb::get_driver_instance();
			SQLString url(CONNECTION_URL);
			Properties properties( { { "user", USER }, { "password", PASSWD }, { "useTls", "true" } });
			this->connection = driver->connect(url, properties);
		}
	}

	void commit() {
		this->connection->commit();
	}

	void rollback() {
		this->connection->rollback();
	}

	void closeConnection() {
		if (this->connection != NULL) {
			if (!this->connection->isClosed()) {
				this->connection->close();
			}
			delete this->connection;
			this->connection = NULL;
		}
	}

private:

	PreparedStatement* prepareStatement(const vector<string> &parameters, string &sql) {
		setParameters(parameters, sql);
		return this->connection->prepareStatement(sql);
	}

};

inline static DatabaseManager DBMANAGER;
