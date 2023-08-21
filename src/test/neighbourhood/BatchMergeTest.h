#pragma once

#include "../../main/neighbourhood/BatchMerge.h"
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

inline void testRandomBatchMergeFidelis() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int bestCost = schedule->cost;
	cout << "Original cost: " << schedule->cost << endl;
	int numImprovements = 0;
	for (int i = 0; i < 1000; i++) {
		executeRandomBatchMergeFidelis(schedule);
		validateSchedule(schedule);

		int scheduleCost = schedule->cost;
		int newCalculatedCost = calculateScheduleCostStateful(schedule);
		assert((scheduleCost == newCalculatedCost) && "Costs should be equal");

		if (schedule->cost < bestCost) {
			bestCost = schedule->cost;
			numImprovements++;
		}
	}
	cout << "Final schedule cost: " << to_string(schedule->cost) << " | improvements: " << numImprovements << endl;
	cout << "Best cost: " << to_string(bestCost) << " | improvements: " << numImprovements << endl;
	delete schedule;
}

inline void testFullBatchMergeQueiroga() {
	Schedule *schedule = createScheduleWithTwdForTest();

	cout << "testFullBatchMergeQueiroga / 1" << endl;

	// Must split some batches before merge
	for (int i = 0; i < 50; i++) {
		executeRandomBatchSplitQueiroga(schedule);
	}
	validateSchedule(schedule);

	int originalCost = schedule->cost;

	BatchMerge batchMerge = executeFullBatchMergeQueiroga(schedule);
	validateSchedule(schedule);

	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "Batch merge object cost: " << to_string(batchMerge.newScheduleCost) << endl;
	cout << "Schedule redundancy cost: " << to_string(schedule->cost) << endl;
	assert((batchMerge.newScheduleCost == schedule->cost) && "Costs should be equal");
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost) << endl;
	assert((batchMerge.newScheduleCost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testFullBatchMergeQueirogaImprovement() {
	Schedule *schedule = createScheduleWithTwdForTest();

	// Must split some batches before merge
	for (int i = 0; i < 50; i++) {
		executeRandomBatchSplitQueiroga(schedule);
	}

	int originalCost = schedule->cost;
	for (int i = 0; i < 100; i++) {
		executeFullBatchMergeQueiroga(schedule);
	}
	cout << "testFullBatchMergeQueirogaImprovement // validating ... " << endl;
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	delete schedule;
}

inline void runBatchMergeTest() {
	cout << "# Running BatchMergeTest.h" << endl;

	cout << endl << ".... testRandomBatchMergeFidelis [STARTED]" << endl;
	testRandomBatchMergeFidelis();
	cout << endl << ".... testFullBatchMergeQueiroga [STARTED]" << endl;
	testFullBatchMergeQueiroga();
	cout << endl << "... testFullBatchMergeQueirogaImprovement [STARTED]" << endl;
	testFullBatchMergeQueirogaImprovement();

	cout << endl << "[OK] BatchMergeTest.h" << endl;
}
