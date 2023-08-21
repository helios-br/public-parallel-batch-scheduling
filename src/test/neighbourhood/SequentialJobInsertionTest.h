#pragma once

#include "../../main/neighbourhood/SequentialJobInsertion.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testFullSequentialJobInsertion() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	executeFullSequentialJobInsertion(schedule, 5);
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	assert((schedule->cost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testSequentialJobInsertionImprovement() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	for (int i = 0; i < 250; i++) {
		executeFullSequentialJobInsertion(schedule, generateRandomNumberInRange(1, 10));
	}
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	delete schedule;
}

inline void runSequentialJobInsertionTest() {
	cout << "# Running SequentialJobInsertionTest.h" << endl;

	cout << endl << ".... testFullSequentialJobInsertion [STARTED]" << endl;
	testFullSequentialJobInsertion();
	cout << endl << ".... testSequentialJobInsertionImprovement [STARTED]" << endl;
	testSequentialJobInsertionImprovement();

	cout << endl << "[OK] SequentialJobInsertionTest.h" << endl;
}
