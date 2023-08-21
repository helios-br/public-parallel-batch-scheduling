#pragma once

#include "../../main/neighbourhood/BatchInsertion.h"
#include "../schedule/ScheduleTestHelper.h"
#include "../../main/schedule/Schedule.h"
#include "../../main/schedule/ScheduleUtil.h"
#include "../../main/schedule/ScheduleCostProcessor.h"
#include "../../main/schedule/ScheduleValidator.h"

#include <iostream>
#include <assert.h>

using namespace std;

inline void validateBatchInsertion(BatchInsertion batchInsertion, bool verifyBatchAssignment) {
	assert((batchInsertion.batch != NULL) && "Batch should not be null");
	assert((batchInsertion.newMachine != NULL) && "Machine should not be null");
	assert((batchInsertion.newMachinePositionIndex >= 0) && "Position is invalid");
	if (verifyBatchAssignment) {
		assert((batchInsertion.batch->machine == batchInsertion.newMachine) && "Batch wrongly assigned");
		//assert((batchInsertion.batch->machineIndexPosition == batchInsertion.newMachinePositionIndex) && "Position is incorrect");
	}
}

inline void testRandomBatchInsertion() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();

	for (int i = 0; i < 10000; i++) {
		// execute
		BatchInsertion batchInsertion = executeRandomBatchInsertion(schedule);

		// verify
		validateBatchInsertion(batchInsertion, true);
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void testMultipleRandomBatchInsertion() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();

	for (int i = 0; i < 10000; i++) {
		// execute
		vector<BatchInsertion> batchInsertions = executeRandomBatchInsertion(schedule, 5);

		// verify
		for (int i = 0; i < batchInsertions.size(); i++) {
			validateBatchInsertion(batchInsertions[i], false);
		}
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void testBatchInsertionScheduleBatch() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();
	int bestCost = schedule->cost;
	cout << "Original cost: " << schedule->cost << endl;
	int numImprovements = 0;

	for (int i = 0; i < 10000; i++) {
		// Remove random batch for reinsertion
		Batch *batch = getRandomBatch(schedule);
		batch->machine->removeBatch(batch);

		// execute
		BatchInsertion batchInsertion = executeBatchInsertion(schedule, batch);

		// verify
		validateBatchInsertion(batchInsertion, true);
		validateSchedule(schedule);

		if (schedule->cost < bestCost) {
			bestCost = schedule->cost;
			numImprovements++;
		}
	}
	cout << "Final cost: " << bestCost << " | improvements: " << numImprovements << endl;
	delete schedule;
}

inline void testExecuteBatchSingletonInsertion() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();

	for (int i = 0; i < 10000; i++) {
		// execute
		BatchInsertion batchInsertion = executeBatchSingletonInsertion(schedule);

		// verify
		validateBatchInsertion(batchInsertion, true);
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void testFullBatchInsertion() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;

	// execute
	BatchInsertion batchInsertion = executeFullBatchInsertion(schedule);

	// verify
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "Batch insertion object cost: " << to_string(batchInsertion.newScheduleCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	assert((batchInsertion.newScheduleCost == schedule->cost) && "Costs should be equal");
	int newCalculatedCost = calculateScheduleCostStateful(schedule);
	cout << "Calculated cost: " << to_string(newCalculatedCost);
	assert((batchInsertion.newScheduleCost == newCalculatedCost) && "Costs should be equal");
	delete schedule;
}

inline void testBatchInsertionScheduleBatchMachine() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();
	int bestCost = schedule->cost;
	cout << "Original cost: " << schedule->cost << endl;
	int numImprovements = 0;

	for (int i = 0; i < 10000; i++) {
		// Remove random batch for reinsertion
		Batch *batch = getRandomBatch(schedule);
		Machine *currentMachine = batch->machine;
		batch->machine->removeBatch(batch);
		calculateMachineCostStatefulAndUpdateScheduleCost(currentMachine, schedule);
		Machine *machine = getRandomMachine(schedule);

		// execute
		BatchInsertion batchInsertion = executeBatchInsertion(schedule, batch, machine);

		// verify
		assert((batchInsertion.newMachine == machine) && "Assigned to wrong machine");
		validateBatchInsertion(batchInsertion, true);
		validateSchedule(schedule);

		if (schedule->cost < bestCost) {
			bestCost = schedule->cost;
			numImprovements++;
		}
	}
	cout << "Final cost: " << bestCost << " | improvements: " << numImprovements << endl;
	delete schedule;
}

inline void testBatchInsertionImprovement() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();
	int originalCost = schedule->cost;

	for (int i = 0; i < 100; i++) {
		// execute
		executeFullBatchInsertion(schedule);
	}

	// verify
	validateSchedule(schedule);
	cout << "Original cost: " << to_string(originalCost) << endl;
	cout << "New schedule cost: " << to_string(schedule->cost) << endl;
	delete schedule;
}

inline void testRandomBatchSequenceInsertionBilyk() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();

	for (int i = 0; i < 10000; i++) {
		// execute
		//cout << "before >>" << endl;
		//printBatchesSequence(schedule);
		vector<BatchInsertion> batchInsertions = executeRandomBatchSequenceInsertionBilyk(schedule, generateRandomNumberInRange(1, 5));
		//cout << "after >>" << endl;
		//printBatchesSequence(schedule);
		// verify
		assert((batchInsertions.size() > 0) && "Vector should not be empty");

		for (int i = 0; i < batchInsertions.size(); i++) {
			validateBatchInsertion(batchInsertions[i], true);
			assert((batchInsertions[i].batch->machineIndexPosition == batchInsertions[i].newMachinePositionIndex) && "Position is incorrect");
			if (i < batchInsertions.size() - 1) {
				assert((batchInsertions[i].newMachine == batchInsertions[i + 1].newMachine) && "Machines are different");
				assert((batchInsertions[i].batch != batchInsertions[i + 1].batch) && "Wrong batch");
				assert((batchInsertions[i].newMachinePositionIndex == batchInsertions[i + 1].newMachinePositionIndex - 1) && "Positions aren't in sequence");
			}
		}
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void testRandomBatchSequenceSwapInsertionBilyk() {
	// setup
	Schedule *schedule = createScheduleWithTwdForTest();
	int totalBatches = schedule->totalBatches();

	for (int i = 0; i < 2500; i++) {
		// execute
		//cout << "before >>" << endl;
		//printBatchesSequence(schedule);
		vector<BatchInsertion> batchInsertions = executeRandomBatchSequenceSwapInsertionBilyk(schedule, generateRandomNumberInRange(1, 3));
		//cout << "after >>" << endl;
		//printBatchesSequence(schedule);

		// verify
		assert((batchInsertions.size() > 0) && "Vector should not be empty");
		assert((schedule->totalBatches() == totalBatches) && "Number of batches should not change");

		for (int i = 0; i < batchInsertions.size(); i++) {
			validateBatchInsertion(batchInsertions[i], true);
			if (i < batchInsertions.size() - 1) {
				if (batchInsertions[i].newMachine == batchInsertions[i + 1].newMachine) {
					assert((batchInsertions[i].newMachinePositionIndex == batchInsertions[i + 1].newMachinePositionIndex - 1) && "Positions aren't in sequence");
					assert((batchInsertions[i].newMachinePositionIndex == batchInsertions[i + 1].newMachinePositionIndex - 1) && "Positions aren't in sequence");
				}
				assert((batchInsertions[i].batch != batchInsertions[i + 1].batch) && "Wrong batch");
			}

		}
		validateSchedule(schedule);
	}
	delete schedule;
}

inline void runBatchInsertionTest() {
	cout << "# Running BatchInsertionTest.h" << endl;

	cout << endl << ".... testRandomBatchInsertion [STARTED]" << endl;
	testRandomBatchInsertion();
	cout << endl << ".... testBatchInsertionScheduleBatch [STARTED]" << endl;
	testBatchInsertionScheduleBatch();
	cout << endl << ".... testExecuteBatchSingletonInsertion [STARTED]" << endl;
	testExecuteBatchSingletonInsertion();
	cout << endl << ".... testBatchInsertionScheduleBatchMachine [STARTED]" << endl;
	testBatchInsertionScheduleBatchMachine();
	cout << endl << ".... testFullBatchInsertion [STARTED]" << endl;
	testFullBatchInsertion();
	cout << endl << ".... testBatchInsertionImprovement [STARTED]" << endl;
	testBatchInsertionImprovement();
	cout << endl << ".... testMultipleRandomBatchInsertion [STARTED]" << endl;
	testMultipleRandomBatchInsertion();
	cout << endl << ".... testRandomBatchSequenceInsertionBilyk [STARTED]" << endl;
	testRandomBatchSequenceInsertionBilyk();
	cout << endl << ".... testRandomBatchSequenceSwapInsertionBilyk [STARTED]" << endl;
	testRandomBatchSequenceSwapInsertionBilyk();

	cout << endl << "[OK] BatchInsertionTest.h" << endl;
}
