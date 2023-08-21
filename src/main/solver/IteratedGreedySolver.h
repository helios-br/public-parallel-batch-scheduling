#pragma once

#include "Solver.h"
#include "../instance/Instance.h"
#include "../schedule/Schedule.h"

class IteratedGreedySolver: public Solver {

public:
	int getSolverInstanceId() {
		return 10;
	}

private:
	Schedule* solveInstance(Instance *instance);
	void executeLocalSearchProcedure(Schedule *schedule);
	void destroyAndConstruct(Schedule *schedule, int destructionSize);
	double calculateConstantTemperature(Instance *instance);
};
