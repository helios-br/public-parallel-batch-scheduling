#pragma once

#include <iostream>
#include "../instance/InstanceTestHelper.h"
#include "../../main/schedule/ScheduleValidator.h"
#include "../../main/solver/Solver.h"
#include "../../main/solver/TimeWindowDecomposition.h"

inline void testSolver() {
	Instance *instance = loadTestInstance();
	Solver *solver = new TimeWindowDecomposition(false);
	for (int i = 0; i < 100; i++) {
		Schedule *schedule = solver->solve(instance);
		validateSchedule(schedule);
		delete schedule;
	}
}

inline void testSolverWithRandomParameters() {
	Instance *instance = loadTestInstance();
	TimeWindowDecomposition *solver = new TimeWindowDecomposition(false);
	Schedule *schedule = solver->solve(instance);
	int bestCost = schedule->cost;
	delete schedule;
	//cout << "twd | thres = " << solver->thres << ", lookAhead = " << solver->lookAhead << " | result: " << schedule->cost << endl;
	for (int i = 0; i < 100; i++) {
		solver = new TimeWindowDecomposition(true);
		schedule = solver->solve(instance);
		validateSchedule(schedule);
		calculateScheduleCostStateful(schedule);
		if (schedule->cost < bestCost) {
			bestCost = schedule->cost;
			//cout << "twd | thres = " << solver->thres << ", lookAhead = " << solver->lookAhead << " | result: " << schedule->cost << endl;
		}
		delete schedule;
		delete solver;
	}
}

inline void runTimeWindowDecompositionSolverTest() {
	cout << "# Running TimeWindowDecompositionSolverTest.h" << endl;

	cout << endl << ".... testSolver [STARTED]" << endl;
	testSolver();
	cout << endl << ".... testSolverWithRandomParameters [STARTED]" << endl;
	testSolverWithRandomParameters();

	cout << "[OK] TimeWindowDecompositionSolverTest.h" << endl;
}
