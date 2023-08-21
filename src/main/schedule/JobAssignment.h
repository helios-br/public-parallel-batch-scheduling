#pragma once

#include <iostream>
#include "Batch.h"
#include "Job.h"
#include "Schedule.h"

inline bool canAddJobToBatch(Batch *batch, Job *job) {
	return job->batch != batch && (job->instanceJob->instanceFamily == batch->instanceFamily) && !batch->isFull();
}

inline void assignJobToBatch(Job *job, Batch *batch, int index) {
	job->batchIndexPosition = index;
	job->assignedToBatch = true;
	job->batch = batch;
}

inline void cleanJobAssignment(Job *job, Batch *batch) {
	if (job->batch == batch) {
		job->batchIndexPosition = -1;
		job->assignedToBatch = false;
		job->batch = NULL;
		job->assignedToMachine = false;
	}
}

inline void confirmAssignmentOfAllJobsToMachine(Batch *batch) {
	for (int j = 0; j < batch->jobs.size(); j++) {
		Job *job = batch->jobs[j];
		if (!job->isDummy()) {
			job->assignedToBatch = true;
			job->assignedToMachine = true;
			batch->machine->schedule->totalAssignedJobs += 1;
		}
	}
}

inline void cancelAssignmentOfAllJobsFromMachine(Batch *batch) {
	for (int j = 0; j < batch->jobs.size(); j++) {
		Job *job = batch->jobs[j];
		if (!job->isDummy()) {
			job->assignedToMachine = false;
			batch->machine->schedule->totalAssignedJobs -= 1;
		}
	}
}

