#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"

class VNSBilyk: public Solver {

public:
	int getSolverInstanceId() {
		return 15;
	}

private:
	Schedule* solveInstance(Instance *instance);
};
