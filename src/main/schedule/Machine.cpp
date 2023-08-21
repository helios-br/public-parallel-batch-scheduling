#include "Machine.h"

#include <climits>
#include "BatchAssignment.h"
#include "JobAssignment.h"

Machine::Machine(InstanceMachine *instanceMachine, int maxBatches, Schedule *schedule) {
	this->instanceMachine = instanceMachine;
	this->batches.assign(maxBatches, &DUMMY_BATCH);
	this->numBatches = 0;
	this->schedule = schedule;
}

Machine::~Machine() {
	for (int b = 0; b < this->batches.size(); b++) {
		Batch *batch = this->batches[b];
		if (!batch->isDummy()) {
			delete batch;
		}
	}
}

void Machine::createBatchWithJobAndInsert(Job *job, int machineIndexPosition) {
	Batch *batch = new Batch(job->instanceJob->instanceFamily, this->instanceMachine->size);
	batch->addJob(job);
	if (machineIndexPosition < this->numBatches) {
		this->insertAtPositionAndDelay(batch, machineIndexPosition);
	} else {
		this->insertAfterLastBatch(batch);
	}
}

void Machine::insertAtFirstAvailablePosition(Batch *batch) {
	int positionIndexToInsert = 0;
	Batch *previousBatch = NULL;
	int availableStartTime = 0;
	int availableProcessingTime = INT_MAX;

	for (int b = 0; b < this->numBatches; b++) {
		Batch *currentBatch = this->batches[b];

		availableProcessingTime = currentBatch->machineStartTime;

		if (previousBatch != NULL) {
			availableStartTime = previousBatch->machineCompletionTime;
			availableProcessingTime = currentBatch->machineStartTime - previousBatch->machineCompletionTime;
		}

		if (availableProcessingTime >= batch->processingTime) {
			int startTime = availableStartTime > batch->releaseTime ? availableStartTime : batch->releaseTime;
			int completionTime = startTime + batch->processingTime;

			if (completionTime <= currentBatch->machineStartTime) {
				positionIndexToInsert = b;
				break;
			}
		}

		if (b == this->numBatches - 1) {
			positionIndexToInsert = b + 1;
			break;
		}

		previousBatch = currentBatch;
	}

	insertAtPosition(batch, positionIndexToInsert);
}

void Machine::insertAtPositionAndDelay(Batch *batch, int index) {
	insertAtPosition(batch, index);
}

void Machine::insertAfterLastBatch(Batch *batch) {
	insertAtPosition(batch, this->numBatches);
}

void Machine::insertAtPosition(Batch *batch, int index) {
	Batch *batchToInsert = batch;
	Batch *tempBatch = NULL;
	int availableStartTime = 0;
	int startTime = 0;

	for (int b = index; b < this->numBatches + 1; b++) {
		tempBatch = this->batches[b];
		this->batches[b] = batchToInsert;
		this->batches[b]->machineIndexPosition = b;

		availableStartTime = b == 0 ? 0 : this->batches[b - 1]->machineCompletionTime;
		startTime = availableStartTime > batchToInsert->releaseTime ? availableStartTime : batchToInsert->releaseTime;
		assignBatchToMachine(batchToInsert, this, b, startTime);
		batchToInsert = tempBatch;
	}

	this->numBatches += 1;
}

void Machine::removeBatch(Batch *batch) {

	// Shift batches to the left to keep the order correct
	for (int b = batch->machineIndexPosition; b < this->numBatches - 1; b++) {
		Batch *nextBatch = this->batches[b + 1];

		this->batches[b] = nextBatch;
		this->batches[b]->machineIndexPosition = b;

		// Improve allocation of "shifed" batches (if possible)
		Batch *previousBatch = b == 0 ? NULL : this->batches[b - 1];
		int availableStartTime = previousBatch == NULL ? 0 : previousBatch->machineCompletionTime;

		if (availableStartTime < nextBatch->machineStartTime) {
			int newStartTime = availableStartTime <= nextBatch->releaseTime ? nextBatch->releaseTime : availableStartTime;
			assignBatchToMachine(nextBatch, this, nextBatch->machineIndexPosition, newStartTime);
		}
	}

	this->batches[this->numBatches - 1] = &DUMMY_BATCH;
	this->numBatches -= 1;
	cleanBatchAssignment(batch);
}
