#pragma once

#include "../../main/neighbourhood/JobInsertion.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void testExecuteJobInsertionFromSmallestBatch() {
	Schedule *schedule = createScheduleWithTwdForTest();
	for (int i = 0; i < 1000; i++) {
		vector<JobInsertion> jobInsertions = executeJobInsertionFromSmallestBatch(schedule);
		validateSchedule(schedule);
		assert((jobInsertions.size() >= 1) && "Must have at least one object");
		for (JobInsertion jobInsertion : jobInsertions) {
			if (jobInsertion.bestBatch != NULL) {
				assert((jobInsertion.job->batch == jobInsertion.bestBatch) && "Wrong batch");
			}
			assert((jobInsertion.job->assignedToBatch) && "Should be assigned to a batch");
		}
	}
	delete schedule;
}

inline void testFullJobInsertion() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	JobInsertion jobInsertion = executeFullJobInsertion(schedule);
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "Batch insertion object cost: " << to_string(jobInsertion.newScheduleCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	assert((jobInsertion.newScheduleCost == schedule->cost) && "Costs should be equal");
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	assert((jobInsertion.newScheduleCost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testFullJobInsertionBilyk() {
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;
	JobInsertion jobInsertion = executeFullJobInsertionBilyk(schedule);
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "Batch insertion object cost: " << to_string(jobInsertion.newScheduleCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	assert((jobInsertion.newScheduleCost == schedule->cost) && "Costs should be equal");
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	assert((jobInsertion.newScheduleCost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testRandomJobInsertion() {
	Schedule *schedule = createScheduleWithTwdForTest();
	for (int i = 0; i < 10000; i++) {
		JobInsertion jobInsertion = executeRandomJobInsertion(schedule);
		validateSchedule(schedule);
		if (jobInsertion.bestBatch != NULL) {
			assert((jobInsertion.job->batch == jobInsertion.bestBatch) && "Wrong batch");
		} else {
			assert((jobInsertion.job->batch->currentSize == 1) && "Should be a singleton batch");
			assert((jobInsertion.job->batch->machine == jobInsertion.bestMachine) && "Machines are not equal");
			assert((jobInsertion.job->batch->machineIndexPosition == jobInsertion.bestMachineIndexPosition) && "Batch is in the wrong position");
		}
		assert((jobInsertion.job->assignedToBatch) && "Should be assigned to a batch");
	}
	delete schedule;
}

inline void testFullJobInsertionImprovement() {
	Schedule *schedule = createScheduleWithTwdForTest();
	validateSchedule(schedule);
	int originalCost = schedule->cost;
	for (int i = 0; i < 100; i++) {
		executeFullJobInsertion(schedule);
		validateSchedule(schedule);
	}
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	delete schedule;
}

inline void runJobInsertionTest() {
	cout << "# Running JobInsertionTest.h" << endl;

	cout << endl << ".... testExecuteJobInsertionFromSmallestBatch [STARTED]" << endl;
	testExecuteJobInsertionFromSmallestBatch();
	cout << endl << ".... testFullJobInsertion [STARTED]" << endl;
	testFullJobInsertion();
	cout << endl << ".... testRandomJobInsertion [STARTED]" << endl;
	testRandomJobInsertion();
	cout << endl << ".... testFullJobInsertionImprovement [STARTED]" << endl;
	testFullJobInsertionImprovement();
	cout << endl << ".... testFullJobInsertionBilyk [STARTED]" << endl;
	testFullJobInsertionBilyk();

	cout << endl << "[OK] JobInsertionTest.h" << endl;
}
