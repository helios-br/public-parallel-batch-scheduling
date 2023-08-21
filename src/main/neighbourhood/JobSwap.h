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

struct JobSwap {

	Schedule *schedule = NULL;
	Job *firstJob = NULL;
	Job *secondJob = NULL;

	Batch *newBatchFirstJob = NULL;
	Batch *newBatchSecondJob = NULL;

	int newScheduleCost = -1;

public:

	JobSwap(Schedule *schedule) {
		this->schedule = schedule;
	}

	void update(Job *firstJob, Batch *newBatchFirstJob, Job *secondJob, Batch *newBatchSecondJob, int newScheduleCost) {
		this->firstJob = firstJob;
		this->newBatchFirstJob = newBatchFirstJob;
		this->secondJob = secondJob;
		this->newBatchSecondJob = newBatchSecondJob;
		this->newScheduleCost = newScheduleCost;
	}
};

inline void applyJobSwap(JobSwap &jobSwap) {
	removeJobAndUpdateBatchAssignment(jobSwap.firstJob, false, true);
	removeJobAndUpdateBatchAssignment(jobSwap.secondJob, false, true);

	addJobAndUpdateBatchAssignment(jobSwap.firstJob, jobSwap.newBatchFirstJob);
	addJobAndUpdateBatchAssignment(jobSwap.secondJob, jobSwap.newBatchSecondJob);

	// Update cost

	calculateMachineCostStatefulAndUpdateScheduleCost(jobSwap.newBatchFirstJob->machine, jobSwap.schedule);
	if (jobSwap.newBatchFirstJob->machine != jobSwap.newBatchSecondJob->machine) {
		calculateMachineCostStatefulAndUpdateScheduleCost(jobSwap.newBatchSecondJob->machine, jobSwap.schedule);
	}
}

inline JobSwap executeRandomJobSwap(Schedule *schedule) {
	JobSwap jobSwap(schedule);

	Job *firstJob = getRandomJob(schedule);
	Batch *firstBatch = firstJob->batch;

	Job *secondJob = NULL;
	Batch *secondBatch = NULL;

	while (true) {
		secondJob = getRandomJobFromFamily(schedule, firstJob->instanceJob->instanceFamily);
		secondBatch = secondJob->batch;

		if (firstBatch == secondBatch) {
			continue;
		}

		break;
	}

	jobSwap.update(firstJob, secondBatch, secondJob, firstBatch, 0);

	// Update schedule

	applyJobSwap(jobSwap);

	return jobSwap;
}

inline JobSwap executeFullJobSwap(Schedule *schedule) {
	JobSwap jobSwap(schedule);
	int bestCost = schedule->cost;

	for (int m = 0; m < schedule->machines.size(); m++) {
		Machine *currentMachine = schedule->machines[m];

		for (int b = 0; b < currentMachine->numBatches; b++) {
			Batch *currentBatch = currentMachine->batches[b];

			for (int j = 0; j < currentBatch->jobs.size(); j++) {
				Job *currentJob = currentBatch->jobs[j];

				if (currentJob->isDummy()) {
					continue;
				}

				removeJobAndUpdateBatchAssignment(currentJob, false, true);

				for (int nm = m; nm < schedule->machines.size(); nm++) {
					Machine *newMachine = schedule->machines[nm];

					int startIndex = nm != m ? 0 : b + 1;

					for (int nb = startIndex; nb < newMachine->numBatches; nb++) {

						// Avoid same batch
						if (nm == m && nb == b) {
							continue;
						}

						Batch *newBatch = newMachine->batches[nb];

						for (int nj = 0; nj < newBatch->jobs.size(); nj++) {
							Job *newJob = newBatch->jobs[nj];

							if (newJob->isDummy()) {
								continue;
							}

							if (!canAddJobToBatch(currentBatch, newJob)) {
								continue;
							}

							removeJobAndUpdateBatchAssignment(newJob, false, true);
							if (!canAddJobToBatch(newBatch, currentJob)) {
								addJobAndUpdateBatchAssignment(newJob, newBatch);
								continue;
							}

							addJobAndUpdateBatchAssignment(currentJob, newBatch);
							addJobAndUpdateBatchAssignment(newJob, currentBatch);

							int newCost = schedule->cost - currentMachine->cost + calculateMachineCostStateless(currentMachine);
							if (currentMachine != newMachine) {
								newCost = newCost - newMachine->cost + calculateMachineCostStateless(newMachine);
							}

							if (newCost < bestCost) {
								bestCost = newCost;
								jobSwap.update(currentJob, newBatch, newJob, currentBatch, newCost);
							}

							removeJobAndUpdateBatchAssignment(newJob, false, true);
							removeJobAndUpdateBatchAssignment(currentJob, false, true);
							addJobAndUpdateBatchAssignment(newJob, newBatch);
						}
					}
				}
				// Rollback
				addJobAndUpdateBatchAssignment(currentJob, currentBatch);
			}
		}
	}

	// Update schedule
	if (jobSwap.firstJob != NULL) {
		applyJobSwap(jobSwap);
	}
	return jobSwap;
}

