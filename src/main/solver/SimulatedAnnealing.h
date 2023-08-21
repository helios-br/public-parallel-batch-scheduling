#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"

class SimulatedAnnealing: public Solver {

	// The parameter setting chosen for SA algorithm were: α = 0.5, nMax=n/10 and T0 = 20,000

	// Initial temperature
	double initialTemperature = 20000;

	// Cooling rate
	double coolingRate = 0.5;

public:
	int getSolverInstanceId() {
		return 11;
	}

private:
	Schedule* solveInstance(Instance *instance);
	Schedule* perturb(Schedule *schedule);
};
