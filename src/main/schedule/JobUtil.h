#pragma once

#include <vector>
#include <iostream>
#include <algorithm>
#include "../instance/InstanceJob.h"
#include "Job.h"

using namespace std;

inline void shuffleJobs(vector<Job *> &originalvector) {
	auto rng = default_random_engine {};
	shuffle(begin(originalvector), end(originalvector), rng);
}

// Methods responsible for sorting jobs by their processing time

inline bool compareJobsByProcessingTimeNonAsc(Job *firstJob, Job *secondJob) {
	if (firstJob->isDummy()) {
		return false;
	} else if (secondJob->isDummy()) {
		return true;
	}
	return firstJob->instanceJob->processingTime > secondJob->instanceJob->processingTime;
}

inline void sortJobsByProcessingTimeNonAsc(vector<Job*> &jobs) {
	sort(jobs.begin(), jobs.end(), compareJobsByProcessingTimeNonAsc);
}

// Printing methods

inline void printJobs(vector<Job*> &jobs) {
	cout << "Jobs size: " << to_string(jobs.size()) << endl;
	for (Job *job : jobs) {
		cout << job->toString() << endl;
	}
}
