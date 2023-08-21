#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"

class TWDBilyk: public Solver {

public:
	int getSolverInstanceId() {
		return 18;
	}

private:
	Schedule* solveInstance(Instance *instance);
};
