#pragma once

#include "../../main/neighbourhood/JobFastInsertion.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testFullJobFastInsertion() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	for (int i = 0; i < 50; i++) {
		executeFullJobFastInsertion(schedule);
		validateSchedule(schedule);
		executeRandomBatchMergeFidelis(schedule);
	}
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	delete schedule;
}

inline void runJobFastInsertionTest() {
	cout << "# Running JobFastInsertionTest.h" << endl;

	cout << endl << ".... testFullJobFastInsertion [STARTED]" << endl;
	testFullJobFastInsertion();

	cout << endl << "[OK] JobFastInsertionTest.h" << endl;
}
