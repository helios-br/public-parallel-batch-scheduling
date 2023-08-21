#pragma once

#include <iostream>
#include <vector>

#include "../schedule/Batch.h"
#include "../schedule/BatchAssignment.h"
#include "../schedule/JobUtil.h"
#include "../schedule/JobAssignment.h"
#include "../schedule/Machine.h"
#include "../schedule/Schedule.h"
#include "../schedule/ScheduleCostProcessor.h"
#include "../schedule/ScheduleValidator.h"
#include "../schedule/ScheduleUtil.h"
#include "../util/NumberUtil.h"

using namespace std;

struct SequentialBatchInsertion {

	Schedule *schedule = NULL;
	Batch *batch = NULL;

	// Old position of this batch

	Machine *originalMachine = NULL;
	int originalMachinePositionIndex = -1;

	// New position of this batch

	Machine *newMachine = NULL;
	int newMachinePositionIndex = -1;
	int newScheduleCost = -1;

	SequentialBatchInsertion(Schedule *schedule, Batch *batch) {
		this->schedule = schedule;
		this->batch = batch;
		this->originalMachine = batch->machine;
		this->originalMachinePositionIndex = batch->machineIndexPosition;
	}

	void update(Machine *newMachine, int newMachinePositionIndex, int newScheduleCost) {
		this->newScheduleCost = newScheduleCost;
		this->newMachine = newMachine;
		this->newMachinePositionIndex = newMachinePositionIndex;
	}
};

inline void applySequentialBatchInsertion(SequentialBatchInsertion &batchInsertion) {
	Machine *previousMachine = batchInsertion.originalMachine;
	batchInsertion.newMachine->insertAtPositionAndDelay(batchInsertion.batch, batchInsertion.newMachinePositionIndex);
	calculateMachineCostStatefulAndUpdateScheduleCost(previousMachine, batchInsertion.schedule);
	if (previousMachine != batchInsertion.newMachine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(batchInsertion.newMachine, batchInsertion.schedule);
	}
}

inline void executeFullSequentialBatchInsertion(Schedule *schedule, int numBatchesToRemove) {
	vector<SequentialBatchInsertion> sequentialBatchInsertions;
	set<Machine*> machinesToUpdateCost;

	// Select a random batch from a random machine
	for (int b = 0; b < numBatchesToRemove; b++) {
		Batch *batch = getRandomBatch(schedule);

		if (!batch->assigned) {
			continue;
		}

		Machine *machine = batch->machine;
		sequentialBatchInsertions.push_back(SequentialBatchInsertion(schedule, batch));
		machine->removeBatch(batch);
		machinesToUpdateCost.insert(machine);
	}

	for (Machine *machine : machinesToUpdateCost) {
		calculateMachineCostStatefulAndUpdateScheduleCost(machine, schedule);
	}

	//  For each removed batch, all insertion possibilities are tested

	for (SequentialBatchInsertion batchInsertion : sequentialBatchInsertions) {
		Batch *batch = batchInsertion.batch;

		for (int m = 0; m < schedule->machines.size(); m++) {
			Machine *machine = schedule->machines[m];

			int partialScheduleCost = schedule->cost - machine->cost;

			for (int b = 0; b <= machine->numBatches; b++) {
				machine->insertAtPositionAndDelay(batch, b);
				int newCost = partialScheduleCost + calculateMachineCostStateless(machine);
				if (batchInsertion.newScheduleCost == -1 || newCost < batchInsertion.newScheduleCost) {
					batchInsertion.update(machine, b, newCost);
				}
				machine->removeBatch(batch);
			}
		}

		// Update schedule
		applySequentialBatchInsertion(batchInsertion);
	}
}
