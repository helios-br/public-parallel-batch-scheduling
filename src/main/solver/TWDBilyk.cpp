#include "TWDBilyk.h"

#include "TimeWindowDecomposition.h"

using namespace std;

/**
 * TWD
 * Scheduling jobs with ready times and precedence constraints on parallel batch machines using metaheuristics
 * Andrew Bilyk, Lars Möncha, Christian Almeder
 */
Schedule* TWDBilyk::solveInstance(Instance *instance) {

	// Run TWD (j,BATC-II-WS) for k = 0.1h,h=1,...,50 to find an initial solution x(j).
	// Choose the initial solution x(k*) that leads to the smallest TWT value.

	Schedule *bestSchedule = NULL;
	Schedule *schedule = NULL;
	Solver *twdSolver = NULL;

	for (int h = 1; h <= 50; h++) {
		twdSolver = new TimeWindowDecomposition(0.1 * h);
		schedule = twdSolver->solve(instance);
		if (bestSchedule == NULL || schedule->cost < bestSchedule->cost) {
			delete bestSchedule;
			bestSchedule = schedule;
			executionController->bestParameter = h;
		} else {
			delete schedule;
		}

		delete twdSolver;
	}

	return bestSchedule;
}
