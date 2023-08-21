#include "Batch.h"

#include <iostream>
#include <climits>
#include "JobAssignment.h"

struct Job;

Batch::Batch(InstanceFamily *instanceFamily, int maxBatchSize) {
	this->instanceFamily = instanceFamily;
	this->maxSize = maxBatchSize;
	this->jobs.assign(maxBatchSize, &DUMMY_JOB);
}

Batch::~Batch() {
	for (int index = 0; index < this->jobs.size(); index++) {
		cleanJobAssignment(this->jobs[index], this);
	}
}

bool Batch::isFull() {
	return this->currentSize == this->maxSize;
}

void Batch::addJobs(vector<Job*> &jobs) {
	for (Job *job : jobs) {
		this->addJob(job);
	}
}

void Batch::addJob(Job *job) {
	for (int index = 0; index < this->jobs.size(); index++) {

		// Verify whether index is available
		if (this->jobs[index]->isDummy()) {

			// Add job to the batch
			this->jobs[index] = job;
			this->currentSize += 1;

			// Update job references
			assignJobToBatch(job, this, index);

			// Define release time
			if (job->instanceJob->releaseTime > this->releaseTime || this->currentSize == 1) {
				this->releaseTime = job->instanceJob->releaseTime;
			}

			// Define processing time
			if (job->instanceJob->processingTime > this->processingTime || this->currentSize == 1) {
				this->processingTime = job->instanceJob->processingTime;
			}

			// Completion time
			this->completionTime = this->releaseTime + this->processingTime;

			return;
		}
	}
}

void Batch::removeJob(Job *jobToBeRemoved) {
	this->jobs[jobToBeRemoved->batchIndexPosition] = &DUMMY_JOB;
	this->currentSize -= 1;
	cleanJobAssignment(jobToBeRemoved, this);

	// Update batch release and processing time redundancies if necessary

	if (jobToBeRemoved->instanceJob->releaseTime == this->releaseTime || jobToBeRemoved->instanceJob->processingTime == this->processingTime) {
		int highestReleaseTime = 0;
		int highestProcessingTime = 0;

		for (int index = 0; index < this->jobs.size(); index++) {
			Job *jobInBatch = this->jobs[index];

			if (!jobInBatch->isDummy()) {
				if (jobInBatch->instanceJob->releaseTime > highestReleaseTime) {
					highestReleaseTime = jobInBatch->instanceJob->releaseTime;
				}
				if (jobInBatch->instanceJob->processingTime > highestProcessingTime) {
					highestProcessingTime = jobInBatch->instanceJob->processingTime;
				}
			}

			this->releaseTime = highestReleaseTime;
			this->processingTime = highestProcessingTime;
			this->completionTime = this->releaseTime + this->processingTime;
		}
	}
}

void Batch::removeAllJobs() {
	vector<Job*> output;
	removeAllJobs(output);
}

void Batch::removeAllJobs(vector<Job*> &output) {
	for (int j = 0; j < this->jobs.size(); j++) {
		Job *job = this->jobs[j];
		if (!job->isDummy()) {
			cleanJobAssignment(job, this);
			this->jobs[j] = &DUMMY_JOB;
			output.push_back(job);
		}
	}

	this->currentSize = 0;
	this->releaseTime = 0;
	this->processingTime = 0;
	this->completionTime = 0;
}

Job* Batch::getJob(int index) {
	int realIndex = 0;
	for (int j = 0; j < this->jobs.size(); j++) {
		Job *job = this->jobs[j];
		if (job->isDummy()) {
			continue;
		}
		if (index == realIndex) {
			return job;
		}
		realIndex++;
	}
	return NULL;
}

int Batch::tardiness() {
	int batchTardiness = 0;
	for (int j = 0; j < this->jobs.size(); j++) {
		Job *job = this->jobs[j];
		if (job->isDummy()) {
			continue;
		}
		if (this->machineCompletionTime > job->instanceJob->dueDate) {
			int jobTardiness = this->machineCompletionTime - job->instanceJob->dueDate;
			if (jobTardiness > batchTardiness) {
				batchTardiness = jobTardiness;
			}
		}
	}
	return batchTardiness;
}

bool Batch::isDummy() {
	return this->instanceFamily == NULL;
}

string Batch::toString() {
	return "{f: " + to_string(this->instanceFamily->id) + ", maxSize: " + to_string(this->maxSize) + "}";
}

