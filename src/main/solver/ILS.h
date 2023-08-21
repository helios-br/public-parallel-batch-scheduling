#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"

class ILS: public Solver {

	// Maximum number of perturbation moves (I_p)
	int maxNumberPerturbationMoves = 3;

public:
	int getSolverInstanceId() {
		return 14;
	}

private:
	Schedule* solveInstance(Instance *instance);
};
