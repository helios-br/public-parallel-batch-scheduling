#include <iostream>
#include "instance/Instance.h"
#include "instance/ChiangInstanceLoader.h"
#include "solver/Solver.h"
#include "solver/SolverUtil.h"
#include "schedule/Schedule.h"
#include "schedule/ScheduleValidator.h"
#include "schedule/ScheduleCostProcessor.h"
//#include "db/SolutionService.h"

using namespace std;

inline void runApplication(char *instanceName, char *solverParam, char *hostname) {
	// Load instance data
	Instance *instance = loadChiangInstance(instanceName);

	// Solve
	Solver *solver = getSolver(solverParam);
	Schedule *schedule = solver->solve(instance);

	// Validate
	validateSchedule(schedule);
	//saveSolution(schedule, solver, hostname);
	printScheduleCostOnly(schedule);

	delete solver;
	delete schedule;
	delete instance;
}

