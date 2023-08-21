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

struct BatchMerge {

	Schedule *schedule = NULL;
	Batch *firstBatch = NULL;
	Batch *secondBatch = NULL;
	Machine *machine = NULL;
	int machineIndexPosition = -1;
	int newScheduleCost = -1;

public:

	BatchMerge(Schedule *schedule) {
		this->schedule = schedule;
	}

	void update(Batch *firstBatch, Batch *secondBatch, int newScheduleCost) {
		this->firstBatch = firstBatch;
		this->secondBatch = secondBatch;
		this->newScheduleCost = newScheduleCost;
	}

	void update(Batch *firstBatch, Batch *secondBatch, Machine *machine, int machineIndexPosition, int newScheduleCost) {
		this->firstBatch = firstBatch;
		this->secondBatch = secondBatch;
		this->machine = machine;
		this->machineIndexPosition = machineIndexPosition;
		this->newScheduleCost = newScheduleCost;
	}
};

inline void applyBatchMergeQueiroga(BatchMerge &batchMerge) {
	vector<Job*> allJobs;

	Batch *firstBatch = batchMerge.firstBatch;
	Batch *secondBatch = batchMerge.secondBatch;

	// Remove batches from machines
	Machine *firstMachine = firstBatch->machine;
	firstBatch->machine->removeBatch(firstBatch);
	Machine *secondMachine = secondBatch->machine;
	secondBatch->machine->removeBatch(secondBatch);

	// Remove jobs from batches
	firstBatch->removeAllJobs(allJobs);
	secondBatch->removeAllJobs(allJobs);

	// Create new batch
	Batch *newBatch = new Batch(firstBatch->instanceFamily, firstBatch->maxSize);
	newBatch->addJobs(allJobs);

	// Insert batch in machine
	batchMerge.machine->insertAtPositionAndDelay(newBatch, batchMerge.machineIndexPosition);
	calculateMachineCostStatefulAndUpdateScheduleCost(batchMerge.machine, batchMerge.schedule);

	if (firstMachine != batchMerge.machine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(firstMachine, batchMerge.schedule);
	}

	if (secondMachine != batchMerge.machine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(secondMachine, batchMerge.schedule);
	}

	delete firstBatch;
	delete secondBatch;
}

/**
 * "Two batches B1 and B2 of same family are randomly selected and they are merged to form a new batch B
 * Then, two new batches B1 and B2 are constructed with jobs selected at random from B"
 * Fidelis, Michele B. Arroyo, Jose Elias C. (2017)
 *
 * "Seleciona aleatoriamente dois lotes da mesma família. Retira-se as tarefas de ambos os lotes e as
 * adiciona ao conjunto N . As tarefas são escolhidas de forma aleatória do conjunto N e então são inseridas novamente
 * aos lotes."
 * Fidelis, Michele B. (Master's Thesis)
 */
inline void applyBatchMergeFidelis(BatchMerge &batchMerge) {
	vector<Job*> allJobs;

	Batch *firstBatch = batchMerge.firstBatch;
	Batch *secondBatch = batchMerge.secondBatch;

	// Detach batches from machine
	Machine *firstMachine = firstBatch->machine;
	int firstBatchMachineIndexPosition = firstBatch->machineIndexPosition;

	Machine *secondMachine = secondBatch->machine;
	int secondBatchMachineIndexPosition = secondBatch->machineIndexPosition;

	firstMachine->removeBatch(firstBatch);
	secondMachine->removeBatch(secondBatch);

	// Remove jobs from each batch
	// Max size is equal for both batches
	firstBatch->removeAllJobs(allJobs);
	secondBatch->removeAllJobs(allJobs);

	// Batches are reconstructed with jobs selected at random
	shuffleJobs(allJobs);
	bool batchFlag = true;

	for (int j = 0; j < allJobs.size(); j++) {
		Job *job = allJobs[j];
		if (batchFlag) {
			firstBatch->addJob(job);
			batchFlag = false;
		} else {
			secondBatch->addJob(job);
			batchFlag = true;
		}
	}

	if (firstMachine == secondMachine) {
		if (firstBatchMachineIndexPosition < secondBatchMachineIndexPosition) {
			firstMachine->insertAtPositionAndDelay(firstBatch, firstBatchMachineIndexPosition);
			secondMachine->insertAtPositionAndDelay(secondBatch, secondBatchMachineIndexPosition);
		} else {
			secondMachine->insertAtPositionAndDelay(secondBatch, secondBatchMachineIndexPosition);
			firstMachine->insertAtPositionAndDelay(firstBatch, firstBatchMachineIndexPosition);
		}
	} else {
		firstMachine->insertAtPositionAndDelay(firstBatch, firstBatchMachineIndexPosition);
		secondMachine->insertAtPositionAndDelay(secondBatch, secondBatchMachineIndexPosition);
	}

	// Update cost
	calculateMachineCostStatefulAndUpdateScheduleCost(firstMachine, batchMerge.schedule);
	if (secondMachine != firstMachine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(secondMachine, batchMerge.schedule);
	}
}

/**
 * "Two batches B1 and B2 of same family are randomly selected and they are merged to form a new batch B
 * Then, two new batches B1 and B2 are constructed with jobs selected at random from B"
 * Fidelis, Michele B. Arroyo, Jose Elias C. (2017)
 */
inline BatchMerge executeRandomBatchMergeFidelis(Schedule *schedule) {
	BatchMerge batchMerge(schedule);

	Batch *firstBatch = getRandomBatch(schedule);
	Batch *secondBatch = NULL;

	while (true) {
		secondBatch = getRandomBatchFromFamily(schedule, firstBatch->instanceFamily);
		if (firstBatch != secondBatch) {
			break;
		}
	}

	batchMerge.update(firstBatch, secondBatch, 0);

	// Update schedule
	applyBatchMergeFidelis(batchMerge);
	return batchMerge;
}

/**
 * "In this neighborhood, here after referred to as merge batch, two different batches are merged (see Fig.6).
 * The new batch is positioned in place of the left most batch between the two involved in the move."
 * Queiroga et al (2020)
 */
inline BatchMerge executeFullBatchMergeQueiroga(Schedule *schedule) {
	BatchMerge batchMerge(schedule);
	int bestCost = schedule->cost;

	vector<Job*> firstBatchJobs;
	vector<Job*> secondBatchJobs;

	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *firstMachine = schedule->machines[m];

		for (int b = 0; b < firstMachine->numBatches; b++) {
			Batch *firstBatch = firstMachine->batches[b];

			if (firstBatch->currentSize == schedule->instance->maxBatchSize) {
				continue;
			}

			// Remove jobs from current batch

			firstMachine->removeBatch(firstBatch);
			firstBatch->removeAllJobs(firstBatchJobs);

			for (int nm = m; nm < schedule->machines.size(); nm++) {
				Machine *secondMachine = schedule->machines[nm];
				int startIndex = nm != m ? 0 : b + 1;

				for (int nb = startIndex; nb < secondMachine->numBatches; nb++) {
					Batch *secondBatch = secondMachine->batches[nb];

					if (firstBatchJobs.size() + secondBatch->currentSize > schedule->instance->maxBatchSize || firstBatch == secondBatch
							|| firstBatch->instanceFamily != secondBatch->instanceFamily) {
						continue;
					}

					// Remove jobs from second batch
					secondMachine->removeBatch(secondBatch);
					secondBatch->removeAllJobs(secondBatchJobs);

					// Create new batch
					Batch *newBatch = new Batch(secondBatch->instanceFamily, secondBatch->maxSize);
					newBatch->addJobs(firstBatchJobs);
					newBatch->addJobs(secondBatchJobs);

					// Insert new batch in "current batch" original place

					firstMachine->insertAtPositionAndDelay(newBatch, b);

					int firstMachineCost = calculateMachineCostStateless(firstMachine);
					int newCost = schedule->cost - firstMachine->cost + firstMachineCost;

					if (firstMachine != secondMachine) {
						newCost = newCost - secondMachine->cost + calculateMachineCostStateless(secondMachine);
					}

					if (newCost < bestCost) {
						bestCost = newCost;
						batchMerge.update(firstBatch, secondBatch, firstMachine, b, newCost);
					}

					firstMachine->removeBatch(newBatch);

					if (firstMachine != secondMachine) {

						// Insert new batch in "other batch" original place

						secondMachine->insertAtPositionAndDelay(newBatch, nb);

						newCost = schedule->cost - firstMachine->cost + calculateMachineCostStateless(firstMachine) - secondMachine->cost
								+ calculateMachineCostStateless(secondMachine);

						if (newCost < bestCost) {
							bestCost = newCost;
							batchMerge.update(firstBatch, secondBatch, secondMachine, nb, newCost);
						}

						secondMachine->removeBatch(newBatch);
					}

					// Restore schedule
					newBatch->removeAllJobs();
					secondBatch->addJobs(secondBatchJobs);
					secondMachine->insertAtPositionAndDelay(secondBatch, nb);
					secondBatchJobs.clear();
					delete newBatch;
				}
			}

			// Rollback
			firstBatch->addJobs(firstBatchJobs);
			firstMachine->insertAtPositionAndDelay(firstBatch, b);
			firstBatchJobs.clear();
		}
	}

	// Update schedule
	if (batchMerge.firstBatch != NULL) {
		applyBatchMergeQueiroga(batchMerge);
	}
	return batchMerge;
}
