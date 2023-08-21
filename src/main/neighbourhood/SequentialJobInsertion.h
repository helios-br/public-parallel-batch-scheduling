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

struct SequentialJobInsertion {

	Schedule *schedule = NULL;
	Job *job = NULL;
	Batch *originalBatch = NULL;

	// New batch of this job

	Batch *bestBatch = NULL;
	int newScheduleCost = -1;
	int bestMachineIndexPosition = -1;
	Machine *bestMachine = NULL;

public:

	SequentialJobInsertion(Schedule *schedule, Job *job) {
		this->schedule = schedule;
		this->job = job;
		this->originalBatch = job->batch;

	}

	void update(Machine *bestMachine, int bestMachineIndexPosition, int newScheduleCost) {
		this->bestMachine = bestMachine;
		this->bestMachineIndexPosition = bestMachineIndexPosition;
		this->newScheduleCost = newScheduleCost;
		// reset
		this->bestBatch = NULL;
	}

	void update(Batch *bestBatch, int newScheduleCost) {
		this->bestBatch = bestBatch;
		this->newScheduleCost = newScheduleCost;
		// reset
		this->bestMachine = NULL;
		this->bestMachineIndexPosition = -1;
	}
};

inline void applySequentialJobInsertion(SequentialJobInsertion &jobInsertion) {
	if (jobInsertion.bestBatch != NULL) {
		addJobAndUpdateBatchAssignment(jobInsertion.job, jobInsertion.bestBatch);
		calculateMachineCostStatefulAndUpdateScheduleCost(jobInsertion.bestBatch->machine, jobInsertion.schedule);
	} else {
		jobInsertion.bestMachine->createBatchWithJobAndInsert(jobInsertion.job, jobInsertion.bestMachineIndexPosition);
		calculateMachineCostStatefulAndUpdateScheduleCost(jobInsertion.bestMachine, jobInsertion.schedule);
	}
}

inline void executeFullSequentialJobInsertion(Schedule *schedule, int numJobsToRemove) {
	vector<SequentialJobInsertion> sequentialJobInsertions;
	set<Machine*> machinesToUpdateCost;

	for (int j = 0; j < numJobsToRemove; j++) {
		Job *job = getRandomJob(schedule);

		if (!job->assignedToBatch) {
			continue;
		}

		machinesToUpdateCost.insert(job->batch->machine);
		sequentialJobInsertions.push_back(SequentialJobInsertion(schedule, job));
		removeJobAndUpdateBatchAssignment(job, true);
	}

	for (Machine *machine : machinesToUpdateCost) {
		calculateMachineCostStatefulAndUpdateScheduleCost(machine, schedule);
	}

	for (SequentialJobInsertion jobInsertion : sequentialJobInsertions) {
		Job *job = jobInsertion.job;

		// The best solution could be the insertion of the job in an existing batch or in a new one
		Batch *newBatch = new Batch(job->instanceJob->instanceFamily, schedule->instance->maxBatchSize);
		int newCost = 0;

		for (int m = 0; m < schedule->machines.size(); m++) {
			Machine *machine = schedule->machines[m];
			int partialScheduleCost = schedule->cost - machine->cost;

			for (int b = 0; b <= machine->numBatches; b++) {

				// 1. Create a new singleton batch and add it to the current position
				newBatch->addJob(job);
				machine->insertAtPositionAndDelay(newBatch, b);
				newCost = partialScheduleCost + calculateMachineCostStateless(machine);

				if (jobInsertion.newScheduleCost == -1 || newCost < jobInsertion.newScheduleCost) {
					jobInsertion.update(machine, b, newCost);
				}

				machine->removeBatch(newBatch);
				newBatch->removeJob(job);

				if (b == machine->numBatches) {
					continue;
				}

				// 2. Try to insert the job in the existing batch
				Batch *batch = machine->batches[b];

				if (jobInsertion.originalBatch == batch || !canAddJobToBatch(batch, job)) {
					continue;
				}

				addJobAndUpdateBatchAssignment(job, batch);
				newCost = partialScheduleCost + calculateMachineCostStateless(machine);
				if (jobInsertion.newScheduleCost == -1 || newCost < jobInsertion.newScheduleCost) {
					jobInsertion.update(batch, newCost);
				}
				removeJobAndUpdateBatchAssignment(job, false);
			}
		}
		delete newBatch;

		// Update schedule
		applySequentialJobInsertion(jobInsertion);
	}
}
