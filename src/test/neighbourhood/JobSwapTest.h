#pragma once

#include "../../main/neighbourhood/JobSwap.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testFullJobSwap() {
	Schedule *schedule = createScheduleWithTwdForTest();
	validateSchedule(schedule);
	int originalCost = schedule->cost;
	JobSwap jobSwap = executeFullJobSwap(schedule);
	validateSchedule(schedule);
	assert((jobSwap.firstJob->assignedToBatch) && "Should be assigned to a batch");
	assert((jobSwap.secondJob->assignedToBatch) && "Should be assigned to a batch");
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "Job swap cost: " << to_string(jobSwap.newScheduleCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	assert((jobSwap.newScheduleCost == schedule->cost) && "Costs should be equal");
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	assert((jobSwap.newScheduleCost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testRandomJobSwap() {
	Schedule *schedule = createScheduleWithTwdForTest();
	for (int i = 0; i < 2000; i++) {
		int totalBatches = schedule->totalBatches();
		JobSwap jobSwap = executeRandomJobSwap(schedule);
		validateSchedule(schedule);
		assert((jobSwap.firstJob->assignedToBatch) && "Should be assigned to a batch");
		assert((jobSwap.secondJob->assignedToBatch) && "Should be assigned to a batch");
		assert((totalBatches == schedule->totalBatches()) && "Number of batches should not differ");
	}
	delete schedule;
}

inline void testFullJobSwapImprovement() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	for (int i = 0; i < 1000; i++) {
		executeFullJobSwap(schedule);
		validateSchedule(schedule);
	}
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	delete schedule;
}

inline void runJobSwapTest() {
	cout << "# Running JobSwapTest.h" << endl;

	cout << endl << ".... testFullJobSwap [STARTED]" << endl;
	testFullJobSwap();
	cout << endl << ".... testRandomJobSwap [STARTED]" << endl;
	testRandomJobSwap();
	cout << endl << ".... testFullJobSwapImprovement [STARTED]" << endl;
	testFullJobSwapImprovement();

	cout << endl << "[OK] JobSwapTest.h" << endl;
}
