#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "DatabaseManager.h"
#include "SolutionRepository.h"
#include "../schedule/Schedule.h"
#include "../schedule/ScheduleCostProcessor.h"
#include "../solver/Solver.h"
#include "../solver/ImprovementData.h"
#include "ProblemInstanceRepository.h"
#include "DatabaseManager.h"

using namespace std;

inline void saveSolution(Schedule *schedule, Solver *solver, string hostname) {
	try {
		DBMANAGER.openConnection();
		ExecutionController *controller = solver->executionController;

		// Retrieve problem instance
		int codProblemInstance = findProblemInstanceByFilename(schedule->instance->instanceName);
		int bestParameter = solver->executionController->bestParameter;

		// Prepare and save solution data
		string executionTimeStr = to_string(solver->executionController->executionTime);
		string numTardyJobsStr = to_string(totalTardyJobs(schedule));
		string solverInstanceStr = to_string(solver->getSolverInstanceId());
		string initialCostStr = to_string(solver->initialCost/(double) schedule->instance->multiplier);
		string totalIterations = to_string(controller->totalIterations);
		string totalDiversifications = to_string(controller->totalDiversifications);
		string bestParameterStr = bestParameter == -1 ? "NULL" : to_string(bestParameter);
		string scheduleCostStr = to_string(schedule->cost/(double) schedule->instance->multiplier);
		saveSolution(solverInstanceStr, to_string(codProblemInstance), executionTimeStr, scheduleCostStr, numTardyJobsStr, hostname, initialCostStr, totalIterations, bestParameterStr, totalDiversifications);

		// Prepare and save all improvement data
		//int codSolution = getLastSolution(solverInstanceStr, to_string(codProblemInstance));
		//saveImprovementData(codSolution, controller->improvements);

		DBMANAGER.commit();
		DBMANAGER.closeConnection();
	} catch (...) {
		DBMANAGER.rollback();
		DBMANAGER.closeConnection();
		throw;
	}
}
