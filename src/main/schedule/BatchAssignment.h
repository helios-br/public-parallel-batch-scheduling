# pragma once

#include <vector>
#include <string>
#include "Machine.h"
#include "Batch.h"
#include "JobAssignment.h"

inline void assignBatchToMachine(Batch *batch, Machine *machine, int indexPosition, int startTime) {
	if (!batch->assigned) {
		batch->machine = machine;
		batch->assigned = true;
		confirmAssignmentOfAllJobsToMachine(batch);
	}
	batch->machineIndexPosition = indexPosition;
	batch->machineStartTime = startTime;
	batch->machineCompletionTime = startTime + batch->processingTime;
}

inline void cleanBatchAssignment(Batch *batch) {
	cancelAssignmentOfAllJobsFromMachine(batch);
	batch->assigned = false;
	batch->machine = NULL;
	batch->machineStartTime = 0;
	batch->machineCompletionTime = 0;
	batch->machineIndexPosition = -1;
}

inline void removeJobAndUpdateBatchAssignment(Job *job, bool deleteBatch, bool reinsertBatchNoMatterWhat) {
	Batch *batch = job->batch;
	int machineIndexPosition = batch->machineIndexPosition;
	Machine *machine = batch->machine;

	if (batch->currentSize == 1) {
		machine->removeBatch(batch);
		batch->removeJob(job);
		if (deleteBatch) {
			delete batch;
		}
		if (reinsertBatchNoMatterWhat) {
			machine->insertAtPositionAndDelay(batch, machineIndexPosition);
		}
	} else {
		if (job->instanceJob->releaseTime == batch->releaseTime || job->instanceJob->processingTime == batch->processingTime) {
			machine->removeBatch(batch);
			batch->removeJob(job);
			machine->insertAtPositionAndDelay(batch, machineIndexPosition);
		} else {
			batch->removeJob(job);
		}
	}
}

inline void removeJobAndUpdateBatchAssignment(Job *job, bool deleteBatch) {
	removeJobAndUpdateBatchAssignment(job, deleteBatch, false);
}

inline void addJobAndUpdateBatchAssignment(Job *job, Batch *batch) {
	Machine *machine = batch->machine;
	int machineIndexPosition = batch->machineIndexPosition;
	if (batch->assigned) {
		machine->removeBatch(batch);
	}
	batch->addJob(job);
	machine->insertAtPositionAndDelay(batch, machineIndexPosition);
}
