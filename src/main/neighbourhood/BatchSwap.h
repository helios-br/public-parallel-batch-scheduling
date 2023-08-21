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

struct BatchSwap {

	Schedule *schedule = NULL;
	Batch *firstBatch = NULL;
	Batch *secondBatch = NULL;
	int newScheduleCost = -1;

public:

	BatchSwap(Schedule *schedule) {
		this->schedule = schedule;
	}

	void update(Batch *firstBatch, Batch *secondBatch, int newScheduleCost) {
		this->firstBatch = firstBatch;
		this->secondBatch = secondBatch;
		this->newScheduleCost = newScheduleCost;
	}
};

inline void applyBatchSwap(BatchSwap &batchSwap) {
	int newPositionFirstBatch = batchSwap.secondBatch->machineIndexPosition;
	Machine *newMachineFirstBatch = batchSwap.secondBatch->machine;
	int newPositionSecondBatch = batchSwap.firstBatch->machineIndexPosition;
	Machine *newMachineSecondBatch = batchSwap.firstBatch->machine;

	// Remove first batch and insert in the new position
	batchSwap.firstBatch->machine->removeBatch(batchSwap.firstBatch);
	newMachineFirstBatch->insertAtPositionAndDelay(batchSwap.firstBatch, newPositionFirstBatch);

	// Remove second batch and insert in the new position
	batchSwap.secondBatch->machine->removeBatch(batchSwap.secondBatch);
	newMachineSecondBatch->insertAtPositionAndDelay(batchSwap.secondBatch, newPositionSecondBatch);

	// Update cost
	calculateMachineCostStatefulAndUpdateScheduleCost(newMachineSecondBatch, batchSwap.schedule);
	if (newMachineFirstBatch != newMachineSecondBatch) {
		calculateMachineCostStatefulAndUpdateScheduleCost(newMachineFirstBatch, batchSwap.schedule);
	}
}

inline BatchSwap executeRandomBatchSwap(Schedule *schedule, Batch *firstBatch) {
	BatchSwap batchSwap(schedule);

	// Find another batch to swap
	while (batchSwap.firstBatch == NULL) {
		Batch *secondBatch = getRandomBatch(schedule);
		if (firstBatch == secondBatch) {
			continue;
		}
		batchSwap.update(firstBatch, secondBatch, 0);
	}

	applyBatchSwap(batchSwap);
	return batchSwap;
}

inline vector<BatchSwap> executeRandomBatchSwapBilyk(Schedule *schedule, int iterations) {
	vector<BatchSwap> batchSwaps;

	// Randomly select two batches from different machines and exchange their positions
	for (int i = 0; i < iterations; i++) {
		BatchSwap batchSwap(schedule);
		Batch *firstBatch = getRandomBatch(schedule);
		Batch *secondBatch = getRandomBatchFromMachine(getRandomMachineException(schedule, firstBatch->machine));
		batchSwap.update(firstBatch, secondBatch, 0);
		applyBatchSwap(batchSwap);
		batchSwaps.push_back(batchSwap);
	}

	return batchSwaps;
}

inline BatchSwap executeBatchSwapBasedOnHighestTardiness(Schedule *schedule) {
	vector<Batch*> candidates;

	// Set candidates for random selection based on their tardiness
	for (Machine *machine : schedule->machines) {
		for (int b = 0; b < machine->numBatches; b++) {
			Batch *batch = machine->batches[b];
			int tardiness = batch->tardiness();
			for (int i = 0; i < tardiness; i++) {
				candidates.push_back(batch);
			}
		}
	}

	Batch *firstBatch = candidates[generateRandomIndex(candidates.size())];
	return executeRandomBatchSwap(schedule, firstBatch);
}

inline BatchSwap executeRandomBatchSwap(Schedule *schedule) {
	return executeRandomBatchSwap(schedule, getRandomBatch(schedule));
}

inline BatchSwap executeFullBatchSwap(Schedule *schedule, bool onlyDifferentMachines) {
	BatchSwap batchSwap(schedule);
	int bestCost = schedule->cost;

	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *currentMachine = schedule->machines[m];

		for (int b = 0; b < currentMachine->numBatches; b++) {
			Batch *currentBatch = currentMachine->batches[b];

			currentMachine->removeBatch(currentBatch);

			for (int nm = m; nm < schedule->machines.size(); nm++) {
				Machine *newMachine = schedule->machines[nm];

				if (onlyDifferentMachines && newMachine == currentMachine) {
					continue;
				}

				int startIndex = nm != m ? 0 : b + 1;

				for (int nb = startIndex; nb < newMachine->numBatches; nb++) {
					Batch *otherBatch = newMachine->batches[nb];

					// Remove batch from its original position
					newMachine->insertAtPositionAndDelay(currentBatch, otherBatch->machineIndexPosition);
					newMachine->removeBatch(otherBatch);
					currentMachine->insertAtPositionAndDelay(otherBatch, b);

					int newCost = schedule->cost - currentMachine->cost + calculateMachineCostStateless(currentMachine);

					if (currentMachine != newMachine) {
						newCost = newCost - newMachine->cost + calculateMachineCostStateless(newMachine);
					}

					if (newCost < bestCost) {
						bestCost = newCost;
						batchSwap.update(currentBatch, otherBatch, newCost);
					}

					currentMachine->removeBatch(otherBatch);
					newMachine->insertAtPositionAndDelay(otherBatch, currentBatch->machineIndexPosition);
					newMachine->removeBatch(currentBatch);
				}
			}
			// Rollback
			currentMachine->insertAtPositionAndDelay(currentBatch, b);
		}
	}

	// Update schedule
	if (batchSwap.firstBatch != NULL) {
		applyBatchSwap(batchSwap);
	}
	return batchSwap;
}

inline BatchSwap executeFullBatchSwap(Schedule *schedule) {
	return executeFullBatchSwap(schedule, false);
}

inline BatchSwap executeFullBatchSwapBilyk(Schedule *schedule) {
	return executeFullBatchSwap(schedule, true);
}
