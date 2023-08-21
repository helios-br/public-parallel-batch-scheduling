#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <assert.h>
#include "../../main/instance/Instance.h"
#include "../../main/instance/ChiangInstanceLoader.h"
#include "../../main/solver/Solver.h"
#include "../../main/solver/TimeWindowDecomposition.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/util/StringUtil.h"

inline Schedule* TWD_SCHEDULE = NULL;

inline Schedule* createScheduleWithTwdForTest() {
	if (TWD_SCHEDULE == NULL) {
		char fileName[] = "INSTANCE-FOR-TEST.txt";
		Instance *instance = loadChiangInstance(fileName);
		Solver *solver = new TimeWindowDecomposition(true);
		TWD_SCHEDULE = solver->solve(instance);
	}
	return cloneSchedule(TWD_SCHEDULE);
}
