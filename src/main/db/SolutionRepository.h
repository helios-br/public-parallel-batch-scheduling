#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "ProblemInstanceRepository.h"
#include "DatabaseManager.h"
#include "../schedule/Schedule.h"
#include "../schedule/ScheduleCostProcessor.h"
#include "../solver/Solver.h"
#include "../solver/ImprovementData.h"
#include "../util/StringUtil.h"

using namespace std;

inline int getLastSolution(string codSolverInstance, string codProblemInstance) {
	string sql = "SELECT MAX(cod) as cod FROM helius.solution WHERE cod_solver_instance = 0? AND cod_problem_instance = 1?";
	vector<string> parameters { codSolverInstance, codProblemInstance };
	ResultSet *resultSet = DBMANAGER.executeQuery(sql, parameters);
	resultSet->first();
	int codSolution = resultSet->getInt("cod");
	delete resultSet;
	return codSolution;
}

inline void saveImprovementData(
		string neighbourhoodOperator,
		string codSolution,
		string iteration,
		string iterationsWithoutImprovement,
		string initialCost,
		string newCost) {
	string sql = "INSERT INTO "
			" helius.improvement_data "
			" ( "
			" cod_solution, "
			" operator, "
			" iteration, "
			" iteration_without_improvement, "
			" cost, "
			" new_cost "
			" ) "
			" VALUES "
			" ( "
			" 0?, "
			" '1?', "
			" 2?, "
			" 3?, "
			" 4?, "
			" 5? "
			" ) ";
	vector<string> parameters { codSolution, neighbourhoodOperator, iteration, iterationsWithoutImprovement, initialCost, newCost };
	DBMANAGER.executeUpdate(sql, parameters);
}

inline void saveImprovementData(int codSolution, vector<ImprovementData> &improvements) {
	string sql = "INSERT INTO "
			" helius.improvement_data "
			" ( "
			" cod_solution, "
			" operator, "
			" iteration, "
			" iteration_without_improvement, "
			" cost, "
			" new_cost "
			" ) "
			" VALUES ";

	string sqlValue = " (0?, '1?', 2?, 3?, 4?, 5? ) ";

	for (int i = 0; i < improvements.size(); i++) {
		string newValue = sqlValue;
		ImprovementData data = improvements[i];
		vector<string> parameters{to_string(codSolution), neighbourhoodToString(data.neighbourhood), to_string(data.iteration),
							to_string(data.iterationsWithoutImprovement), to_string(data.initialCost), to_string(data.newCost)};
		setParameters(parameters, newValue);
		sql += (newValue + (i < improvements.size() - 1? "," : ""));
	}

	DBMANAGER.executeUpdate(sql, vector<string>());
}

inline void saveSolution(
		string codSolverInstance,
		string codProblemInstance,
		string executionTime,
		string solutionCost,
		string numTardyJobs,
		string hostname,
		string initialSolutionCost,
		string totalIterations,
		string bestParameter,
		string totalDiversifications) {
	string sql = "INSERT INTO "
			"	helius.solution"
			"("
			"	cod_solver_instance, "
			"	cod_problem_instance, "
			"	execution_time, "
			"	cost, "
			"	num_tardy_jobs, "
			"   execution_from, "
			"   initial_solution_cost, "
			"   total_iterations, "
			"   best_parameter, "
			"   total_diversifications "
			")"
			" VALUES "
			"("
			"	0?, "
			"	1?, "
			"	2?, "
			"	3?, "
			"	4?, "
			"   '5?', "
			"   6?, "
			"   7?, "
			"   8?, "
			"   9? "
			")";
	vector<string> parameters { codSolverInstance, codProblemInstance, executionTime, solutionCost, numTardyJobs, hostname, initialSolutionCost, totalIterations, bestParameter, totalDiversifications };
	DBMANAGER.executeUpdate(sql, parameters);
}
