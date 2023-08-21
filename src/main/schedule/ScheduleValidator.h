#pragma once

#include <iostream>
#include <string>
#include <assert.h>

#include "Batch.h"
#include "JobUtil.h"
#include "Machine.h"
#include "Schedule.h"
#include "ScheduleCostProcessor.h"
#include "../util/NumberUtil.h"

using namespace std;

inline void validateJobsInBatch(Batch *batch, bool isBatchAssignedToMachine) {
	int jobCounter = 0;
	int highestReleaseTime = 0;
	int highestProcessingTime = 0;

	for (int j = 0; j < batch->jobs.size(); j++) {
		Job *job = batch->jobs[j];
		InstanceJob *instanceJob = job->instanceJob;

		if (job->isDummy()) {
			assert((!job->assignedToBatch) && "Job should be assigned to a batch");
			assert((!job->assignedToMachine) && "Job should be assigned to a machine");
			continue;
		}

		assert((job->batch == batch) && "Batch redundancy is wrong");
		assert((instanceJob->instanceFamily->id == batch->instanceFamily->id) && "Job and batch family don't match");
		assert((job->assignedToBatch) && "Job should be assigned to a batch");

		if (isBatchAssignedToMachine) {
			assert((job->assignedToMachine) && "Job should be assigned to a machine");
			assert((instanceJob->releaseTime <= job->batch->machineStartTime) && "Job release time should be higher than batch start time");
		}

		assert((job->batchIndexPosition >= 0) && "Job should have a batch index");

		int jobCompletionTime = instanceJob->releaseTime + instanceJob->processingTime;
		assert((instanceJob->releaseTime <= job->batch->releaseTime) && "Batch release time is wrong");
		assert((instanceJob->processingTime <= job->batch->processingTime) && "Batch processing time is wrong");
		assert((jobCompletionTime <= job->batch->completionTime) && "Batch completion time is wrong");

		if (isBatchAssignedToMachine) {
			assert((jobCompletionTime <= job->batch->machineCompletionTime) && "Job completion time should be lower than machine completion time");
		}

		if (job->instanceJob->releaseTime > highestReleaseTime) {
			highestReleaseTime = job->instanceJob->releaseTime;
		}

		if (job->instanceJob->processingTime > highestProcessingTime) {
			highestProcessingTime = job->instanceJob->processingTime;
		}

		jobCounter++;
	}

	assert((jobCounter == batch->currentSize) && "Batch current size is wrong");
	assert((batch->releaseTime == highestReleaseTime) && "Batch release time is wrong");
	assert((batch->processingTime == highestProcessingTime) && "Batch processing time is wrong");
	assert((batch->completionTime == highestReleaseTime + highestProcessingTime) && "Batch completion time is wrong");
}

inline void validateSchedule(Schedule *schedule) {
	int scheduleCost = schedule->cost;
	int realCost = calculateScheduleCostStateful(schedule);
	assert((scheduleCost == realCost) && "costs don't match (schedule)");
	assert((schedule->totalAssignedJobs == schedule->instance->instanceJobs.size()) && "there are unassigned jobs (schedule)");

	for (Job *job : schedule->jobs) {
		string jobStr = ":job-" + to_string(job->instanceJob->id);
		assert((job->assignedToMachine) && "job is not assigned");
		assert((job->batch != NULL) && "batch redundancy is null");
		assert((job->batchIndexPosition >= 0) && "batch index position is invalid");
		assert((!job->isDummy()) && "there are dummy jobs in the family jobs list");
	}

	for (Machine *machine : schedule->machines) {
		string machineStr = "machine-" + to_string(machine->instanceMachine->id);
		string tag = "[validade:" + machineStr + "]";

		for (int b = 0; b < machine->batches.size(); b++) {
			Batch *batch = machine->batches[b];

			if (b >= machine->numBatches) {
				assert((batch->isDummy()) && "this batch should be dummy");
				continue;
			}

			assert((batch->currentSize > 0) && "size should be higher than zero");

			// Verify batches' order
			for (int kb = 0; kb < machine->numBatches; kb++) {
				if (b == kb) {
					continue;
				}

				Batch *otherBatch = machine->batches[kb];
				assert((otherBatch->machine == batch->machine) && "Machine redundancy is wrong");
				if (kb < b) {
					assert((otherBatch->machineStartTime < batch->machineStartTime) && "The order of the batches is wrong!");
					assert((otherBatch->machineStartTime + otherBatch->processingTime <= batch->machineStartTime) && "The order of the batches is wrong!");
				} else {
					assert((batch->machineStartTime < otherBatch->machineStartTime) && "The order of the batches is wrong!");
					assert((batch->machineStartTime + batch->processingTime <= otherBatch->machineStartTime) && "The order of the batches is wrong!");
				}
			}

			assert((batch->currentSize >= 0 && batch->currentSize <= batch->maxSize) && "current size is invalid");
			assert((!batch->isDummy()) && "there are dummy batches");
			assert((batch->completionTime > 0) && "batch completion time is wrong");
			assert((batch->processingTime > 0) && "batch processing time is wrong");
			assert((batch->releaseTime >= 0) && "batch release time is wrong");
			assert((batch->completionTime > batch->releaseTime) && "batch release time is after completion time");
			assert((batch->machine != NULL) && "machine not found");
			assert((batch->machineIndexPosition >= 0 && batch->machineIndexPosition < machine->numBatches) && "machine position invalid");
			assert((batch->machineStartTime >= batch->releaseTime) && "invalid machine start time");
			assert((batch->machineCompletionTime > batch->machineStartTime) && "Batch machine start time is wrong");
			assert((batch->machineCompletionTime == batch->machineStartTime + batch->processingTime) && "Completion time is incorrect");

			// For each job of this batch

			validateJobsInBatch(batch, true);
		}
	}
}
