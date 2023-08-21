#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <mariadb/conncpp.hpp>
#include "DatabaseManager.h"

using namespace std;
using namespace sql;

inline int findProblemInstanceByFilename(string filename) {
	string sql = "SELECT cod FROM helius.problem_instance WHERE UPPER(name) = UPPER('0?')";
	vector<string> parameters { filename };
	ResultSet *result = DBMANAGER.executeQuery(sql, parameters);
	int codProblemInstance = result->next() ? result->getInt("cod") : -1;
	result->close();
	delete result;
	return codProblemInstance;
}

inline void saveProblemInstance(
		string fileName,
		string author,
		string numFamilies,
		string numMachines,
		string numJobs,
		string numJobsPerFamily,
		string maxBatches,
		string alpha,
		string beta,
		string instanceId) {
	string sql = "INSERT INTO "
			"	helius.problem_instance"
			"("
			"	author, "
			"	name, "
			"	num_machines, "
			"	num_families, "
			"	num_jobs, "
			"	num_jobs_per_family, "
			"	alpha, "
			"	beta, "
			"	instance_id"
			")"
			" VALUES "
			"("
			"	'0?', "
			"	'1?', "
			"	2?, "
			"	3?, "
			"	4?, "
			"	5?, "
			"	6?, "
			"	7?, "
			"	8?"
			")";
	vector<string> parameters { author, fileName, numMachines, numFamilies, numJobs, numJobsPerFamily, alpha, beta, instanceId };
	DBMANAGER.executeUpdate(sql, parameters);
}
