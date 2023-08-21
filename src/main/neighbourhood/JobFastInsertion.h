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

inline void executeFullJobFastInsertion(Schedule *schedule) {
	bool scheduleUpdated = false;
	bool currentBatchDeleted = false;

	for (Machine *currentMachine : schedule->machines) {
		for (Batch *currentBatch : currentMachine->batches) {
			currentBatchDeleted = false;

			if (currentBatch->isDummy()) {
				// dummy batches only appear at the end of the list
				break;
			}

			for (Job *job : currentBatch->jobs) {

				if (currentBatchDeleted) {
					break;
				}

				scheduleUpdated = false;

				if (job->isDummy()) {
					continue;
				}

				// Remove job from batch even if its size is 1
				removeJobAndUpdateBatchAssignment(job, false, true);

				int currentMachineCost = calculateMachineCostStateless(currentMachine);
				int currentScheduleCost = schedule->cost - currentMachine->cost + currentMachineCost;
				int newScheduleCost = 0;
				int newMachineCost = 0;

				for (Machine *newMachine : schedule->machines) {
					newMachineCost = currentMachine != newMachine ? newMachine->cost : currentMachineCost;

					for (Batch *newBatch : newMachine->batches) {

						if (newBatch->isDummy()) {
							// dummy batches only appear at the end of the list
							break;
						}

						if (newBatch == currentBatch) {
							continue;
						}

						// Create a batch to be used as a singleton (with only one job). Inserted between previous batches
						Batch *singletonBatch = new Batch(currentBatch->instanceFamily, schedule->instance->maxBatchSize);

						// 1. Create a new singleton batch and add it to the current position in the machine
						singletonBatch->addJob(job);
						newMachine->insertAtPositionAndDelay(singletonBatch, newBatch->machineIndexPosition);

						newScheduleCost = currentScheduleCost - newMachineCost + calculateMachineCostStateless(newMachine);

						if (newScheduleCost < schedule->cost) {
							scheduleUpdated = true;
							break;
						} else {
							// singleBatch is deleted
							removeJobAndUpdateBatchAssignment(job, true);
						}

						// 2. Try to insert the job in the existing batch
						if (!canAddJobToBatch(newBatch, job)) {
							continue;
						}

						addJobAndUpdateBatchAssignment(job, newBatch);
						newScheduleCost = currentScheduleCost - newMachineCost + calculateMachineCostStateless(newMachine);

						if (newScheduleCost < schedule->cost) {
							scheduleUpdated = true;
							break;
						} else {
							removeJobAndUpdateBatchAssignment(job, false);
						}
					} // end new-machine-batch

					if (scheduleUpdated) {

						// Remove empty batch
						if (currentBatch->currentSize == 0 && currentBatch->assigned) {
							currentMachine->removeBatch(currentBatch);
							delete currentBatch;
							currentBatchDeleted = true;
						}

						// Calculate new cost
						calculateMachineCostStatefulAndUpdateScheduleCost(currentMachine, schedule);
						if (currentMachine != newMachine) {
							calculateMachineCostStatefulAndUpdateScheduleCost(newMachine, schedule);
						}
						validateSchedule(schedule);
						break;
					}
				} // end new-machine

				if (!scheduleUpdated) {
					addJobAndUpdateBatchAssignment(job, currentBatch);
				}
			} // end current-machine-batch-job
		} // end current-machine-batch
	} // end current-machine
}
