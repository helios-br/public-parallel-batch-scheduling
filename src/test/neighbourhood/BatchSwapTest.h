#pragma once

#include "../../main/neighbourhood/BatchSwap.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"
#include "../../main/util/NumberUtil.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testFullBatchSwap() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	BatchSwap batchSwap = executeFullBatchSwap(schedule);
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "Batch swap object cost: " << to_string(batchSwap.newScheduleCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	assert((batchSwap.newScheduleCost == schedule->cost) && "Costs should be equal");
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	assert((batchSwap.newScheduleCost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testRandomBatchSwap() {
	Schedule *schedule = createScheduleWithTwdForTest();
	for (int i = 0; i < 1000; i++) {
		executeRandomBatchSwap(schedule);
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void testRandomBatchSwapBilyk() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int totalBatches = schedule->totalBatches();
	for (int i = 0; i < 500; i++) {
		int iterations = generateRandomNumberInRange(1, 3);
		vector<BatchSwap> batchSwaps = executeRandomBatchSwapBilyk(schedule, iterations);
		validateSchedule(schedule);
		assert((schedule->totalBatches() == totalBatches) && "Number of batches inconsistent");
	}
	delete schedule;
}

inline void testBatchSwapBasedOnHighestTardiness() {
	Schedule *schedule = createScheduleWithTwdForTest();
	for (int i = 0; i < 1000; i++) {
		executeBatchSwapBasedOnHighestTardiness(schedule);
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void testFullBatchSwapImprovement() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	for (int i = 0; i < 1000; i++) {
		executeFullBatchSwap(schedule);
	}
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	delete schedule;
}

inline void runBatchSwapTest() {
	cout << "# Running BatchSwapTest.h" << endl;

	cout << endl << ".... testFullBatchSwap [STARTED]" << endl;
	testFullBatchSwap();
	cout << endl << ".... testRandomBatchSwap [STARTED]" << endl;
	testRandomBatchSwap();
	cout << endl << ".... testBatchSwapBasedOnHighestTardiness [STARTED]" << endl;
	testBatchSwapBasedOnHighestTardiness();
	cout << endl << ".... testFullBatchSwapImprovement [STARTED]" << endl;
	testFullBatchSwapImprovement();
	cout << endl << ".... testRandomBatchSwapBilyk [STARTED]" << endl;
	testRandomBatchSwapBilyk();

	cout << endl << "[OK] BatchSwapTest.h" << endl;
}
