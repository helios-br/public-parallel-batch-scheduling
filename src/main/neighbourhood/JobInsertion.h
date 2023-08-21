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

struct JobInsertion {

	Schedule *schedule = NULL;
	Job *job = NULL;
	int newScheduleCost = -1;

	// New batch of this job
	Batch *bestBatch = NULL;

	// In case of singleton batch
	int bestMachineIndexPosition = -1;
	Machine *bestMachine = NULL;

public:

	JobInsertion(Schedule *schedule) {
		this->schedule = schedule;
	}

	void update(Job *job, Machine *bestMachine, int bestMachineIndexPosition, int newScheduleCost) {
		this->job = job;
		this->bestMachine = bestMachine;
		this->bestMachineIndexPosition = bestMachineIndexPosition;
		this->newScheduleCost = newScheduleCost;
		// reset
		this->bestBatch = NULL;
	}

	void update(Job *job, Batch *bestBatch, int newScheduleCost) {
		this->job = job;
		this->bestBatch = bestBatch;
		this->newScheduleCost = newScheduleCost;
		// reset
		this->bestMachine = NULL;
		this->bestMachineIndexPosition = -1;
	}
};

inline void applyJobInsertion(JobInsertion &jobInsertion) {
	Machine *machineToUpdate = jobInsertion.job->batch != NULL && jobInsertion.job->batch->machine != NULL ? jobInsertion.job->batch->machine : NULL;

	if (machineToUpdate != NULL) {
		removeJobAndUpdateBatchAssignment(jobInsertion.job, true);
		calculateMachineCostStatefulAndUpdateScheduleCost(machineToUpdate, jobInsertion.schedule);
	}

	if (jobInsertion.bestBatch != NULL) {
		addJobAndUpdateBatchAssignment(jobInsertion.job, jobInsertion.bestBatch);
		calculateMachineCostStatefulAndUpdateScheduleCost(jobInsertion.bestBatch->machine, jobInsertion.schedule);
	} else {
		if (jobInsertion.job->assignedToBatch) {

			// Important! In case of batch exclusion and the insertion occours in the same machine

			if (jobInsertion.bestMachine == machineToUpdate && jobInsertion.bestMachineIndexPosition > machineToUpdate->numBatches) {
				jobInsertion.bestMachineIndexPosition = machineToUpdate->numBatches;
			}
		}

		jobInsertion.bestMachine->createBatchWithJobAndInsert(jobInsertion.job, jobInsertion.bestMachineIndexPosition);
		calculateMachineCostStatefulAndUpdateScheduleCost(jobInsertion.bestMachine, jobInsertion.schedule);
	}
}

inline JobInsertion executeRandomJobInsertion(Schedule *schedule) {
	JobInsertion jobInsertion(schedule);

	// Select job randomly
	Job *firstJob = getRandomJob(schedule);

	// New batch for the job
	Batch *newBatch = NULL;

	InstanceFamily *family = firstJob->batch->instanceFamily;
	int firstIndex = schedule->firstJobIndex(family);
	int lastIndex = schedule->lastJobIndex(family);

	for (int j = firstIndex; j <= lastIndex; j++) {
		Job *job = schedule->jobs[j];
		if (canAddJobToBatch(job->batch, firstJob)) {
			newBatch = job->batch;
			break;
		}
	}

	if (newBatch != NULL) {
		jobInsertion.update(firstJob, newBatch, 0);
	} else {
		Machine *machine = getRandomMachine(schedule);
		jobInsertion.update(firstJob, machine, generateRandomIndex(machine->numBatches), 0);
	}

	// Update schedule
	applyJobInsertion(jobInsertion);
	return jobInsertion;
}

/**
 * Insert deteached job (not assigned to any batch) in the best position of the schedule
 */
inline JobInsertion executeJobInsertion(Schedule *schedule, Job *detachedJob, bool allowSingletonBatch) {
	JobInsertion jobInsertion(schedule);

	int currentCost = schedule->cost;
	Batch *singletonBatch = new Batch(detachedJob->instanceJob->instanceFamily, schedule->instance->maxBatchSize);

	// Find best possible position to insert the job

	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *machine = schedule->machines[m];
		int originalMachineCost = machine->cost;

		for (int b = 0; b <= machine->numBatches; b++) {

			int newCost = 0;

			if (allowSingletonBatch) {

				// 1. Create a new singleton batch and add it to the current position in the machine
				singletonBatch->addJob(detachedJob);
				machine->insertAtPositionAndDelay(singletonBatch, b);

				newCost = currentCost - originalMachineCost + calculateMachineCostStateless(machine);

				if (jobInsertion.newScheduleCost == -1 || newCost < jobInsertion.newScheduleCost) {
					jobInsertion.update(detachedJob, machine, b, newCost);
				}

				removeJobAndUpdateBatchAssignment(detachedJob, false);

			}

			if (b == machine->numBatches) {
				continue;
			}

			// 2. Try to insert the job in the existing batch
			Batch *newBatch = machine->batches[b];

			if (!canAddJobToBatch(newBatch, detachedJob)) {
				continue;
			}

			addJobAndUpdateBatchAssignment(detachedJob, newBatch);
			newCost = currentCost - originalMachineCost + calculateMachineCostStateless(machine);

			if (jobInsertion.newScheduleCost == -1 || newCost < jobInsertion.newScheduleCost) {
				jobInsertion.update(detachedJob, newBatch, newCost);
			}

			removeJobAndUpdateBatchAssignment(detachedJob, false);
		}
	}

	// Update schedule
	applyJobInsertion(jobInsertion);
	return jobInsertion;
}

inline vector<JobInsertion> executeJobInsertionFromSmallestBatch(Schedule *schedule) {
	vector<JobInsertion> jobInsertions;
	Batch *batch = getBatchWithSmallestNumberOfJobs(schedule);
	batch->machine->removeBatch(batch);
	vector<Job*> allJobs(batch->jobs.begin(), batch->jobs.end());

	// Cancel all job assignments

	delete batch;

	// Insert jobs in the best possible position

	for (Job *job : allJobs) {
		if (!job->isDummy()) {
			JobInsertion jobInsertion = executeJobInsertion(schedule, job, true);
			jobInsertions.push_back(jobInsertion);
		}
	}

	return jobInsertions;
}

/**
 * singletonBatchConfig:
 * 0 - no singleton batch
 * 1 - test singleton b. in all positions
 * 2 - test singleton b. on the last position of the machine only
 */
inline JobInsertion executeFullJobInsertion(Schedule *schedule, int singletonBatchConfig) {
	JobInsertion jobInsertion(schedule);
	int bestCost = schedule->cost;

	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *currentMachine = schedule->machines[m];

		for (int b = 0; b < currentMachine->numBatches; b++) {
			Batch *currentBatch = currentMachine->batches[b];

			// Create a batch to be used as a singleton (with only one job). Inserted between previous batches
			Batch *singletonBatch = new Batch(currentBatch->instanceFamily, schedule->instance->maxBatchSize);

			for (int j = 0; j < currentBatch->jobs.size(); j++) {
				Job *currentJob = currentBatch->jobs[j];

				if (currentJob->isDummy()) {
					continue;
				}

				// Remove job from batch even if its size is 1
				removeJobAndUpdateBatchAssignment(currentJob, false, false);

				int currentMachineCost = calculateMachineCostStateless(currentMachine);
				int currentCost = schedule->cost - currentMachine->cost + currentMachineCost;
				int newCost = 0;
				int newMachineCost = 0;

				for (int nm = 0; nm < schedule->machines.size(); nm++) {
					Machine *newMachine = schedule->machines[nm];

					for (int nb = 0; nb <= newMachine->numBatches; nb++) {

						newMachineCost = currentMachine != newMachine ? newMachine->cost : currentMachineCost;

						if ((singletonBatchConfig == 1) || (singletonBatchConfig == 2 && nb == newMachine->numBatches)) {

							// 1. Create a new singleton batch and add it to the current position in the machine
							singletonBatch->addJob(currentJob);
							newMachine->insertAtPositionAndDelay(singletonBatch, nb);

							newCost = currentCost - newMachineCost + calculateMachineCostStateless(newMachine);

							if (newCost < bestCost) {
								bestCost = newCost;
								jobInsertion.update(currentJob, newMachine, nb, newCost);
							}

							removeJobAndUpdateBatchAssignment(currentJob, false);
						}

						if (nb == newMachine->numBatches) {
							continue;
						}

						// 2. Try to insert the job in the existing batch
						Batch *newBatch = newMachine->batches[nb];
						if (newBatch == currentBatch || !canAddJobToBatch(newBatch, currentJob)) {
							continue;
						}

						// Bilyk: "Only batches that start after the ready time of the job are considered."

						if (singletonBatchConfig == 2 && newBatch->machineStartTime < currentJob->instanceJob->releaseTime) {
							continue;
						}

						addJobAndUpdateBatchAssignment(currentJob, newBatch);
						newCost = currentCost - newMachineCost + calculateMachineCostStateless(newMachine);

						if (newCost < bestCost) {
							bestCost = newCost;
							jobInsertion.update(currentJob, newBatch, newCost);
						}

						removeJobAndUpdateBatchAssignment(currentJob, false);
					}
				}

				// Rollback
				if (currentBatch->currentSize == 0) {
					currentBatch->addJob(currentJob);
					currentMachine->insertAtPositionAndDelay(currentBatch, b);
				} else {
					addJobAndUpdateBatchAssignment(currentJob, currentBatch);
				}
			}

			delete singletonBatch;
		}
	}

	// Update schedule
	if (jobInsertion.job != NULL) {
		applyJobInsertion(jobInsertion);
	}
	return jobInsertion;
}

inline JobInsertion executeFullJobInsertionMedeiros(Schedule *schedule) {
	return executeFullJobInsertion(schedule, 1);
}

inline JobInsertion executeFullJobInsertionQueiroga(Schedule *schedule) {
	return executeFullJobInsertion(schedule, 0);
}

inline JobInsertion executeFullJobInsertionBilyk(Schedule *schedule) {
	return executeFullJobInsertion(schedule, 2);
}
