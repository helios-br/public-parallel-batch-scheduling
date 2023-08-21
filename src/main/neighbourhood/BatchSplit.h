#pragma once

#include <iostream>
#include <vector>
#include <set>

#include "../schedule/Machine.h"
#include "../schedule/Batch.h"
#include "../schedule/BatchAssignment.h"
#include "../schedule/JobUtil.h"
#include "../schedule/JobAssignment.h"
#include "../schedule/Machine.h"
#include "../schedule/Schedule.h"
#include "../schedule/ScheduleUtil.h"
#include "../schedule/ScheduleCostProcessor.h"
#include "../schedule/ScheduleValidator.h"
#include "../util/NumberUtil.h"

using namespace std;

struct BatchSplit {

	Schedule *schedule = NULL;
	Batch *batch = NULL;
	Machine *firstMachine = NULL;
	int firstIndexMachine = -1;
	Machine *secondMachine = NULL;
	int secondIndexMachine = -1;

public:

	BatchSplit(Schedule *schedule) {
		this->schedule = schedule;
	}

	void update(Batch *batch) {
		this->batch = batch;
	}

	void update(Batch *batch, Machine *firstMachine, int firstIndexMachine, Machine *secondMachine, int secondIndexMachine) {
		this->batch = batch;
		this->firstMachine = firstMachine;
		this->firstIndexMachine = firstIndexMachine;
		this->secondMachine = secondMachine;
		this->secondIndexMachine = secondIndexMachine;
	}
};

inline void applyBatchSplitFidelis(BatchSplit &batchSplit) {
	vector<Job*> allJobs;

	// Detach batch from machine
	Batch *batch = batchSplit.batch;
	Machine *originalMachine = batch->machine;
	originalMachine->removeBatch(batch);

	// Remove jobs from batch
	batch->removeAllJobs(allJobs);

	// Create new batches and insert jobs randomly
	Batch *newFirstBach = new Batch(batch->instanceFamily, batch->maxSize);
	Batch *newSecondBach = new Batch(batch->instanceFamily, batch->maxSize);

	int cutPointIndex = generateRandomNumberInRange(1, allJobs.size() - 1);
	shuffleJobs(allJobs);

	for (int j = 0; j < allJobs.size(); j++) {
		Job *job = allJobs[j];
		if (j < cutPointIndex) {
			newFirstBach->addJob(job);
		} else {
			newSecondBach->addJob(job);
		}
	}

	batchSplit.firstMachine->insertAtPositionAndDelay(newFirstBach, batchSplit.firstIndexMachine);
	batchSplit.secondMachine->insertAtPositionAndDelay(newSecondBach, batchSplit.secondIndexMachine);

	// Update cost
	calculateMachineCostStatefulAndUpdateScheduleCost(originalMachine, batchSplit.schedule);
	if (batchSplit.firstMachine != originalMachine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(batchSplit.firstMachine, batchSplit.schedule);
	}
	if (batchSplit.secondMachine != originalMachine && batchSplit.secondMachine != batchSplit.firstMachine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(batchSplit.secondMachine, batchSplit.schedule);
	}

	delete batch;
}

inline void applyBatchSplitQueiroga(BatchSplit &batchSplit) {
	vector<Job*> allJobs;

	// Detach batch from machine
	Batch *batch = batchSplit.batch;
	Machine *originalMachine = batch->machine;
	int originalMachineIndexPosition = batch->machineIndexPosition;
	originalMachine->removeBatch(batch);

	// Remove jobs from batch
	batch->removeAllJobs(allJobs);

	// Create new batches and insert jobs randomly
	Batch *newFirstBach = new Batch(batch->instanceFamily, batch->maxSize);
	Batch *newSecondBach = new Batch(batch->instanceFamily, batch->maxSize);

	int cutPointIndex = generateRandomNumberInRange(1, allJobs.size() - 1);
	sortJobsByProcessingTimeNonAsc(allJobs);

	for (int j = 0; j < allJobs.size(); j++) {
		Job *job = allJobs[j];
		if (j < cutPointIndex) {
			newFirstBach->addJob(job);
		} else {
			newSecondBach->addJob(job);
		}
	}

	// Keep this order! newFirstBach will take newSecondBach's place  [ b1, b2, newFirstBach, newSecondBach, ... ]
	originalMachine->insertAtPositionAndDelay(newSecondBach, originalMachineIndexPosition);
	originalMachine->insertAtPositionAndDelay(newFirstBach, originalMachineIndexPosition);

	// Update cost
	calculateMachineCostStatefulAndUpdateScheduleCost(originalMachine, batchSplit.schedule);

	delete batch;
}

/**
 * "A randomly selected batch is split in two newbatches. Each batch is randomly inserted into a machine"
 * Fidelis, Michele B. Arroyo, Jose Elias C. (2017)
 *
 * "Um lote é escolhido de forma aleatória e então ocorre a divisão
 * do lote em dois novos lotes. Primeiramente verifica-se o número de tarefas do
 * lote para então aleatoriamente escolher o número de tarefas em cada novo lote.
 * Após a criação dos lotes esses são inseridos de forma aleatória em máquinas
 * também escolhidas ao acaso."
 * Fidelis, Michele B. (Master's Thesis)
 */
inline BatchSplit executeRandomBatchSplitFidelis(Schedule *schedule) {
	BatchSplit batchSplit(schedule);
	Batch *batch = NULL;

	do {
		batch = getRandomBatch(schedule);
	} while (batch->currentSize < 2);

	Machine *firstMachine = getRandomMachine(schedule);
	int firstIndexMachine = generateRandomIndex(firstMachine->numBatches);

	Machine *secondMachine = getRandomMachine(schedule);
	int secondIndexMachine = generateRandomIndex(secondMachine->numBatches);

	batchSplit.update(batch, firstMachine, firstIndexMachine, secondMachine, secondIndexMachine);

	applyBatchSplitFidelis(batchSplit);
	return batchSplit;
}

/*
 * "At first, a batch Bi and a cut point [2,|Bi|−1] are selected.
 * Next, assuming that Bi is in non-ascending order w.r.t. the processing time, Bi is divided into two new batches B′i and B′′i,
 * where B′i receives the first t jobs of Bi and B′′i receives the remaining ones.
 * B′i and B′′i are then included, in this order, inplace of Bi in the sequence B (Bi is replaced by the new batches)"
 * Queiroga et al (2020)
 *
 */
inline BatchSplit executeRandomBatchSplitQueiroga(Schedule *schedule) {
	BatchSplit batchSplit(schedule);
	Batch *batch = NULL;

	do {
		batch = getRandomBatch(schedule);
	} while (batch->currentSize < 2);

	batchSplit.update(batch);

	applyBatchSplitQueiroga(batchSplit);
	return batchSplit;
}

/**
 * Randomly select a batch and split it into two batches. The first one remains on the
 * same position and the second one is inserted on a randomly selected different machine.
 * Repeat this procedure l times
 */
inline vector<BatchSplit> executeRandomBatchSplitBilyk(Schedule *schedule, int iterations) {
	vector<BatchSplit> batchSplits;

	for (int i = 0; i < iterations; i++) {

		BatchSplit batchSplit(schedule);
		Batch *batch = NULL;

		do {
			batch = getRandomBatch(schedule);
		} while (batch->currentSize < 2);

		Machine *firstMachine = batch->machine;
		int firstIndexMachine = batch->machineIndexPosition;

		Machine *secondMachine = getRandomMachineException(schedule, batch->machine);
		int secondIndexMachine = generateRandomIndex(secondMachine->numBatches);

		batchSplit.update(batch, firstMachine, firstIndexMachine, secondMachine, secondIndexMachine);

		applyBatchSplitFidelis(batchSplit);
		batchSplits.push_back(batchSplit);
	}

	return batchSplits;
}

inline vector<BatchSplit> executeRandomBatchSplitBilyk(Schedule *schedule) {
	return executeRandomBatchSplitBilyk(schedule, 1);
}

inline BatchSplit executeRandomBatchSplitMedeiros(Schedule *schedule) {
	BatchSplit batchSplit = BatchSplit(schedule);

	// Find machine with lowest cost
	Machine *machineWithLowestCost = schedule->machines[0];
	for (int m = 1; m < schedule->machines.size(); m++) {
		if (schedule->machines[m]->cost < machineWithLowestCost->cost) {
			machineWithLowestCost = schedule->machines[m];
		}
	}

	Batch *batch = NULL;

	do {
		batch = getRandomBatchFromMachine(machineWithLowestCost);
	} while (batch->currentSize < 2);

	batchSplit.update(batch);
	applyBatchSplitQueiroga(batchSplit);
	return batchSplit;
}
