#pragma once

#include "TimeWindowDecomposition.h"
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "../schedule/Batch.h"
#include "../schedule/JobUtil.h"
#include "../schedule/Machine.h"
#include "../schedule/ScheduleCostProcessor.h"
#include "../util/NumberUtil.h"

struct JobBatchIndex {

	int releaseTime = -1;
	double batchIndex = -1;
};

inline Batch* createBatch(int *bestBatchSolution, Job **familyJobs, int maxBatchSize) {
	Batch *batch = new Batch(familyJobs[0]->instanceJob->instanceFamily, maxBatchSize); // @suppress("Symbol is not resolved")
	for (int i = 0; i < maxBatchSize; i++) {
		int jobIndex = bestBatchSolution[i];
		if (jobIndex != -1) {
			batch->addJob(familyJobs[jobIndex]);
		}
	}
	return batch;
}

// Methods responsible for sorting jobs by their assignment and priority

inline bool compareJobsByAssignmentAndPriority(Job *firstJob, Job *secondJob) {
	if (firstJob->instanceJob->instanceFamily != secondJob->instanceJob->instanceFamily) {
		return firstJob->instanceJob->instanceFamily->id < secondJob->instanceJob->instanceFamily->id;
	}

	if (firstJob->assignedToMachine != secondJob->assignedToMachine) {
		return secondJob->assignedToMachine;
	}

	return firstJob->priority > secondJob->priority;
}

inline void sortJobsByAssignmentAndPriority(vector<Job*> &jobs) {
	sort(jobs.begin(), jobs.end(), compareJobsByAssignmentAndPriority);
}

inline int computeAverageProcessingTimeRemainingJobs(vector<Job*> &allJobs) {
	int totalProcessingTimeRemainingJobs = 0;
	int remainingJobs = 0;

	for (Job *job : allJobs) {
		if (!job->assignedToMachine) {
			totalProcessingTimeRemainingJobs += job->instanceJob->processingTime;
			remainingJobs += 1;
			job->assignedToBatch = false;
		}
	}

	return totalProcessingTimeRemainingJobs / remainingJobs;
}

inline void computeJobsPriority(vector<Job*> &allJobs, int initialTime, int timeLimit, double divisor) {

	// I(t) = w_j/p_j exp (- (d_j - p_j - t + (r_j - t)) / k * p')

	// k: look-ahead parameter / k = 0.1h, h = 1, ..., 50
	// t: time when the machine becomes available
	// p': average processing time of the remaining jobs

	for (Job *job : allJobs) {
		if (!job->assignedToBatch && job->instanceJob->releaseTime <= timeLimit) {
			int rj_t = max(0, job->instanceJob->releaseTime - initialTime);
			int dividend1 = job->instanceJob->dueDate + rj_t;
			int dividend2 = job->instanceJob->processingTime + initialTime;
			double dividend = dividend1 > dividend2 ? dividend1 - dividend2 : 0;
			double exp1 = (dividend == 0) ? 1.0 : exp(-1.0 * (dividend / divisor)); // @suppress("Ambiguous problem")
			job->priority = job->weightPriority * exp1;
		} else {
			job->priority = -1.0;
		}
	}

	sortJobsByAssignmentAndPriority(allJobs);
}

inline double computeBatchPriority(
		vector<Job*> &allJobs,
		int maxBatchSize,
		Job **jobs,
		int initialTime,
		int timeLimit,
		double divisor,
		int currentSolutionSize,
		int releaseTime,
		vector<JobBatchIndex> &batchIndex) {

	// I(t) = (n_b / B) * SUM j in J(b) { (wj / p_j) exp ( - (d_j - p_j -t + (r_b - t) ) / k * p'  ) }
	// J(b): set of jobs contained in batch b
	// r_b: maximum ready time of these jobs (in b); = max j in J(b) r_j
	// n_b: number of jobs of batch b
	// B: maximum batch size

	int rb_t = 0;
	double value = 0.0;

	rb_t = (releaseTime > initialTime) ? (releaseTime - initialTime) : 0;

	for (int i = 0; i < currentSolutionSize; i++) {
		Job *job = jobs[i];
		int batchIndexReleaseTime = batchIndex[job->instanceJob->id].releaseTime;
		double index = batchIndex[job->instanceJob->id].batchIndex;

		if (releaseTime != batchIndexReleaseTime) {
			int dividend = max(0, job->instanceJob->dueDate + rb_t - job->instanceJob->processingTime - initialTime);
			double exp1 = (dividend == 0) ? 1.0 : exp(-1.0 * (dividend / divisor)); // @suppress("Ambiguous problem")
			index = job->weightPriority * exp1;
			batchIndex[job->instanceJob->id].releaseTime = releaseTime;
			batchIndex[job->instanceJob->id].batchIndex = index;
		}

		value += index;
	}

	return (currentSolutionSize / (double) maxBatchSize) * value;
}

inline Batch* findBestBatch(vector<Job*> &allJobs, int maxBatchSize, int numberFamilies, int initialTime, int timeLimit, double divisor, int thres) {
	double valueOfBestBatch = -1;

	// Derive the set M'(f, t, delta-t, thres) by choosing not more than the first thres jobs of each family from M
	Job *familyJobsSubset[numberFamilies][thres];
	for (int i = 0; i < numberFamilies; i++) {
		for (int j = 0; j < thres; j++) {
			familyJobsSubset[i][j] = NULL;
		}
	}

	int totalUnassignedJobsInSubset[numberFamilies] { 0 };

	for (Job *job : allJobs) {
		int familyId = job->instanceJob->instanceFamily->id;
		if (job->priority != -1 && totalUnassignedJobsInSubset[familyId] < thres) {
			familyJobsSubset[familyId][totalUnassignedJobsInSubset[familyId]] = job;
			totalUnassignedJobsInSubset[familyId] += 1;
		}
	}

	int bestBatchSolution[maxBatchSize];
	int batchSolution[maxBatchSize];
	fill(batchSolution, batchSolution + maxBatchSize, -1);

	Job *jobsInSolution[maxBatchSize];
	for (int j = 0; j < maxBatchSize; j++) {
		jobsInSolution[j] = NULL;
	}

	int bestFamily = -1;

	//auto inicio = std::chrono::high_resolution_clock::now();
	// Consider all possible batch combinations from jobs of M' and assess each potential batch using the BATC-II index

	vector<JobBatchIndex> batchIndex(allJobs.size(), JobBatchIndex());

	for (int f = 0; f < numberFamilies; f++) {
		int totalUnassignedJobs = totalUnassignedJobsInSubset[f];

		if (totalUnassignedJobs == 0) {
			continue;
		}

		int lastIndexOriginalVector = totalUnassignedJobs - 1;
		int nextSolutionIndex = 0;
		int lastItemInSolution = -1;
		int currentSolutionSize = 0;

		int releaseTime = -1;
		bool updateReleaseTime = false;

		Job *removedJob = NULL;
		Job *jobBefore = NULL;
		Job *newJob = NULL;

		while (true) {
			jobBefore = NULL;
			newJob = NULL;

			// Insert item on next available position
			int i = nextSolutionIndex++;
			batchSolution[i] = ++lastItemInSolution;

			if (!updateReleaseTime) {
				jobBefore = jobsInSolution[i];
				updateReleaseTime = jobBefore != NULL && jobBefore->instanceJob->releaseTime >= releaseTime;
			}

			jobsInSolution[i] = familyJobsSubset[f][lastItemInSolution];

			if (!updateReleaseTime) {
				newJob = jobsInSolution[i];
				updateReleaseTime = newJob != NULL && newJob->instanceJob->releaseTime > releaseTime;
			}

			currentSolutionSize++;

			if (updateReleaseTime) {
				releaseTime = 0;
				for (int i = 0; i < currentSolutionSize; i++) {
					int jobReleaseTime = (*(jobsInSolution + i))->instanceJob->releaseTime;
					if (jobReleaseTime > releaseTime) {
						releaseTime = jobReleaseTime;
					}
				}
			}

			// Verify solution cost
			double value = computeBatchPriority(allJobs, maxBatchSize, jobsInSolution, initialTime, timeLimit, divisor, currentSolutionSize, releaseTime,
					batchIndex);
			updateReleaseTime = false;

			if (value > valueOfBestBatch) {
				valueOfBestBatch = value;
				copy(batchSolution, batchSolution + maxBatchSize, bestBatchSolution);
				bestFamily = f;
			}

			removedJob = NULL;

			if (lastItemInSolution == lastIndexOriginalVector) {
				nextSolutionIndex--;
				batchSolution[nextSolutionIndex] = -1;
				removedJob = jobsInSolution[nextSolutionIndex];
				jobsInSolution[nextSolutionIndex] = NULL;
				currentSolutionSize--;
				nextSolutionIndex--;
				if (nextSolutionIndex < 0) {
					break;
				}
				lastItemInSolution = batchSolution[nextSolutionIndex];
				currentSolutionSize--;
				updateReleaseTime = removedJob != NULL && removedJob->instanceJob->releaseTime >= releaseTime;
			} else if (currentSolutionSize == maxBatchSize) {
				nextSolutionIndex--;
				currentSolutionSize--;
				batchSolution[nextSolutionIndex] = -1;
				removedJob = jobsInSolution[nextSolutionIndex];
				jobsInSolution[nextSolutionIndex] = NULL;
				updateReleaseTime = removedJob != NULL && removedJob->instanceJob->releaseTime >= releaseTime;
			}

		}
	}

	return createBatch(bestBatchSolution, familyJobsSubset[bestFamily], maxBatchSize);
}
