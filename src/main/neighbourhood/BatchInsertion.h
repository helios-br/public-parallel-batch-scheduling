#pragma once

#include <iostream>
#include <vector>

#include "../schedule/Schedule.h"
#include "../schedule/Batch.h"
#include "../schedule/BatchAssignment.h"
#include "../schedule/JobUtil.h"
#include "../schedule/JobAssignment.h"
#include "../schedule/Machine.h"
#include "../schedule/ScheduleCostProcessor.h"
#include "../schedule/ScheduleValidator.h"
#include "../schedule/ScheduleUtil.h"
#include "../util/NumberUtil.h"

using namespace std;

struct BatchInsertion {

	Schedule *schedule = NULL;
	Batch *batch = NULL;
	int newScheduleCost = -1;

	// New position of this batch

	Machine *newMachine = NULL;
	int newMachinePositionIndex = -1;

public:

	BatchInsertion(Schedule *schedule) {
		this->schedule = schedule;
	}

	void update(Batch *batch, Machine *newMachine, int newMachinePositionIndex, int newScheduleCost) {
		this->batch = batch;
		this->newMachine = newMachine;
		this->newMachinePositionIndex = newMachinePositionIndex;
		this->newScheduleCost = newScheduleCost;
	}
};

inline void applyBatchInsertion(BatchInsertion &batchInsertion) {
	Machine *previousMachine = batchInsertion.batch->machine;

	// Remove batch from machine
	if (previousMachine != NULL) {
		previousMachine->removeBatch(batchInsertion.batch);
	}

	if (batchInsertion.newMachinePositionIndex < batchInsertion.newMachine->numBatches) {
		batchInsertion.newMachine->insertAtPositionAndDelay(batchInsertion.batch, batchInsertion.newMachinePositionIndex);
	} else {
		batchInsertion.newMachine->insertAfterLastBatch(batchInsertion.batch);
	}

	// Update previous machine cost
	if (previousMachine != NULL) {
		calculateMachineCostStatefulAndUpdateScheduleCost(previousMachine, batchInsertion.schedule);
	}

	// Update new machine cost
	if (previousMachine == NULL || previousMachine != batchInsertion.newMachine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(batchInsertion.newMachine, batchInsertion.schedule);
	}
}

/**
 * Randomly select a position on a machine. Remove a sequence of at
 * most l batches and insert either this partial sequence or the
 * reversed one on another machine at a randomly selected position
 */
inline vector<BatchInsertion> executeRandomBatchSequenceInsertionBilyk(Schedule *schedule, int maxSequenceSize) {
	vector<BatchInsertion> batchInsertions;

	// Add first batch of the sequence
	BatchInsertion batchInsertion(schedule);
	Batch *firstBatch = getRandomBatch(schedule);
	Machine *newMachine = getRandomMachineException(schedule, firstBatch->machine);
	int newMachineIndexPosition = generateRandomIndex(newMachine->numBatches + 1);

	// Insert other batches of the sequence
	for (int b = firstBatch->machineIndexPosition, i = 0; b < firstBatch->machine->numBatches && i < maxSequenceSize; b++, i++) {
		BatchInsertion newBatchInsertion(schedule);
		newBatchInsertion.update(firstBatch->machine->batches[b], newMachine, newMachineIndexPosition + i, 0);
		batchInsertions.push_back(newBatchInsertion);
	}

	// Update schedule
	for (int i = 0; i < batchInsertions.size(); i++) {
		applyBatchInsertion(batchInsertions[i]);
	}

	return batchInsertions;
}

inline vector<BatchInsertion> executeRandomBatchSequenceInsertionBilyk(Schedule *schedule) {
	return executeRandomBatchSequenceInsertionBilyk(schedule, 1);
}

/**
 * Randomly select two positions on different machines and exchange the
 * batch sequences starting from that position of at most l length
 */
inline vector<BatchInsertion> executeRandomBatchSequenceSwapInsertionBilyk(Schedule *schedule, int maxSequenceSize) {
	vector<BatchInsertion> batchInsertions;

	// Set first sequence
	BatchInsertion batchInsertion(schedule);
	Batch *firstSequenceBatch = getRandomBatch(schedule);
	Machine *newMachine = getRandomMachineException(schedule, firstSequenceBatch->machine);
	int newMachineIndexPosition = generateRandomIndex(newMachine->numBatches + 1);

	// Insert batches of the first sequence
	for (int b = firstSequenceBatch->machineIndexPosition, i = 0; b < firstSequenceBatch->machine->numBatches && i < maxSequenceSize; b++, i++) {
		BatchInsertion newBatchInsertion(schedule);
		newBatchInsertion.update(firstSequenceBatch->machine->batches[b], newMachine, newMachineIndexPosition + i, 0);
		batchInsertions.push_back(newBatchInsertion);
	}

	// Set second sequence
	Batch *secondSequenceBatch = getRandomBatchFromMachine(newMachine);
	Machine *originalMachine = firstSequenceBatch->machine;
	int newOriginalMachineIndexPosition = generateRandomIndex(originalMachine->numBatches + 1 - batchInsertions.size());

	// Insert batches of the second sequence
	for (int b = secondSequenceBatch->machineIndexPosition, i = 0; b < secondSequenceBatch->machine->numBatches && i < maxSequenceSize; b++, i++) {
		BatchInsertion newBatchInsertion(schedule);
		newBatchInsertion.update(secondSequenceBatch->machine->batches[b], originalMachine, newOriginalMachineIndexPosition + i, 0);
		batchInsertions.push_back(newBatchInsertion);
	}

	// Update schedule
	for (int i = 0; i < batchInsertions.size(); i++) {
		applyBatchInsertion(batchInsertions[i]);
	}

	return batchInsertions;
}

/**
 * A batch is randomly chosen, removed from its original position and
 * inserted in a new random position
 */
inline vector<BatchInsertion> executeRandomBatchInsertion(Schedule *schedule, int iterations) {
	vector<BatchInsertion> batchInsertions;

	for (int i = 0; i < iterations; i++) {
		BatchInsertion batchInsertion(schedule);

		while (batchInsertion.batch == NULL) {
			Batch *firstBatch = getRandomBatch(schedule);
			Machine *newMachine = getRandomMachine(schedule);
			int newMachineIndexPosition = generateRandomIndex(newMachine->numBatches + 1);
			if (newMachine == firstBatch->machine && newMachineIndexPosition == firstBatch->machineIndexPosition) {
				continue;
			}
			batchInsertion.update(firstBatch, newMachine, newMachineIndexPosition, 0);
		}

		// Update schedule
		applyBatchInsertion(batchInsertion);
		batchInsertions.push_back(batchInsertion);
	}

	return batchInsertions;
}

inline BatchInsertion executeRandomBatchInsertion(Schedule *schedule) {
	return executeRandomBatchInsertion(schedule, 1)[0];
}

/**
 * The batch is inserted in the best position of the machine
 */
inline BatchInsertion executeBatchInsertion(Schedule *schedule, Batch *batch, Machine *newMachine, int newMachineIndexPosition) {
	BatchInsertion batchInsertion(schedule);
	batchInsertion.update(batch, newMachine, newMachineIndexPosition, 0);
	applyBatchInsertion(batchInsertion);
	return batchInsertion;
}

/**
 * The batch is inserted in the best position of the machine
 */
inline BatchInsertion executeBatchInsertion(Schedule *schedule, Batch *batch, Machine *newMachine) {
	BatchInsertion batchInsertion(schedule);

	// Calculates temporary schedule cost
	Machine *currentMachine = batch->machine;
	int originalMachineIndexPosition = batch->machineIndexPosition;
	int originalCurrentMachineCost = 0;
	int newCurrentMachineCost = 0;

	if (currentMachine != NULL) {
		originalCurrentMachineCost = currentMachine->cost;
		currentMachine->removeBatch(batch);
		newCurrentMachineCost = calculateMachineCostStateless(currentMachine);
	}

	int currentCost = schedule->cost - originalCurrentMachineCost + newCurrentMachineCost;

	for (Machine *machine : schedule->machines) {

		if (newMachine == NULL || machine == newMachine) {
			int machineCost = machine->cost;

			for (int b = 0; b <= machine->numBatches; b++) {
				machine->insertAtPositionAndDelay(batch, b);

				int newCost = currentCost - machineCost + calculateMachineCostStateless(machine);
				if (batchInsertion.batch == NULL || newCost < batchInsertion.newScheduleCost) {
					batchInsertion.update(batch, machine, b, newCost);
				}

				machine->removeBatch(batch);
			}
		}
	}

	if (currentMachine != NULL) {
		currentMachine->insertAtPositionAndDelay(batch, originalMachineIndexPosition);
	}

	// Update schedule
	applyBatchInsertion(batchInsertion);
	return batchInsertion;
}

/**
 * The batch is inserted in the best position of the schedule
 */
inline BatchInsertion executeBatchInsertion(Schedule *schedule, Batch *batch) {
	return executeBatchInsertion(schedule, batch, NULL);
}

/**
 * A random job is randomly chosen and removed from its batch to form a new one (singleton)
 * The new current solution is obtained by inserting the new batch into the best position of the schedule
 */
inline BatchInsertion executeBatchSingletonInsertion(Schedule *schedule) {
	Job *job = getRandomJob(schedule);
	Machine *originalMachine = job->batch->machine;
	Batch *newSingletonBatch = new Batch(job->batch->instanceFamily, job->batch->maxSize);
	removeJobAndUpdateBatchAssignment(job, true);
	calculateMachineCostStatefulAndUpdateScheduleCost(originalMachine, schedule);
	newSingletonBatch->addJob(job);
	return executeBatchInsertion(schedule, newSingletonBatch);
}

inline BatchInsertion executeFullBatchInsertion(Schedule *schedule) {
	BatchInsertion batchInsertion(schedule);
	int bestCost = schedule->cost;

	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *currentMachine = schedule->machines[m];

		for (int b = 0; b < currentMachine->numBatches; b++) {
			Batch *currentBatch = currentMachine->batches[b];

			// Remove batch from its original position
			currentMachine->removeBatch(currentBatch);

			int currentMachineCost = calculateMachineCostStateless(currentMachine);
			int currentCost = schedule->cost - currentMachine->cost + currentMachineCost;

			for (int nm = 0; nm < schedule->machines.size(); nm++) {
				Machine *newMachine = schedule->machines[nm];
				int newMachineCost = currentMachine != newMachine ? newMachine->cost : currentMachineCost;

				for (int nb = 0; nb < newMachine->numBatches + 1; nb++) {

					// Avoid duplicated moves
					int indexDifference = b - nb;
					if (m == nm && indexDifference >= 0 && indexDifference <= 1) {
						continue;
					}

					newMachine->insertAtPositionAndDelay(currentBatch, nb);
					int newCost = currentCost - newMachineCost + calculateMachineCostStateless(newMachine);

					if (newCost < bestCost) {
						bestCost = newCost;
						batchInsertion.update(currentBatch, newMachine, nb, newCost);
					}

					newMachine->removeBatch(currentBatch);
				}
			}

			// Rollback
			currentMachine->insertAtPositionAndDelay(currentBatch, b);
		}
	}

	// Update schedule
	if (batchInsertion.batch != NULL) {
		applyBatchInsertion(batchInsertion);
	}

	return batchInsertion;
}
