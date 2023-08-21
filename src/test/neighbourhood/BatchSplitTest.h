#pragma once

#include "../../main/neighbourhood/BatchSplit.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"
#include "../../main/util/NumberUtil.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testRandomBatchSplitFidelis() {
	Schedule *schedule = createScheduleWithTwdForTest();
	cout << "Original cost: " << schedule->cost << endl;
	for (int i = 0; i < 100; i++) {
		int totalBatches = schedule->totalBatches();
		executeRandomBatchSplitFidelis(schedule);
		validateSchedule(schedule);
		assert((schedule->totalBatches() == totalBatches + 1) && "number of batches inconsistent");
		//cout << "num of batches: " << schedule->totalBatches() << ", cost: " << schedule->cost << endl;
	}
	delete schedule;
}

inline void testRandomBatchSplitQueiroga() {
	Schedule *schedule = createScheduleWithTwdForTest();
	cout << "Original cost: " << schedule->cost << endl;
	for (int i = 0; i < 100; i++) {
		int totalBatches = schedule->totalBatches();
		executeRandomBatchSplitQueiroga(schedule);
		validateSchedule(schedule);
		assert((schedule->totalBatches() == totalBatches + 1) && "number of batches inconsistent");
		//cout << "num of batches: " << schedule->totalBatches() << ", cost: " << schedule->cost << endl;
	}
	delete schedule;
}

inline void testRandomBatchSplitBilyk() {
	Schedule *schedule = createScheduleWithTwdForTest();
	cout << "Original cost: " << schedule->cost << endl;
	for (int i = 0; i < 20; i++) {
		int totalBatches = schedule->totalBatches();
		int iterations = generateRandomNumberInRange(1, 3);
		printBatchesSequence(schedule);
		vector<BatchSplit> batchSplits = executeRandomBatchSplitBilyk(schedule, iterations);
		printBatchesSequence(schedule);
		validateSchedule(schedule);
		assert((schedule->totalBatches() == totalBatches + iterations) && "number of batches inconsistent");
		assert((batchSplits.size() == iterations) && "vector size is wrong");
	}
	delete schedule;
}

inline void runBatchSplitTest() {
	cout << "# Running BatchSplitTest.h" << endl;

	cout << endl << ".... testRandomBatchSplitFidelis [STARTED]" << endl;
	testRandomBatchSplitFidelis();
	cout << endl << ".... testRandomBatchSplitQueiroga [STARTED]" << endl;
	testRandomBatchSplitQueiroga();
	cout << endl << ".... testRandomBatchSplitBilyk [STARTED]" << endl;
	testRandomBatchSplitBilyk();

	cout << endl << "[OK] BatchSplitTest.h" << endl;
}
