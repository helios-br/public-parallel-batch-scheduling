#pragma once

#include <vector>
#include <string>
#include <bits/stdc++.h>
#include <algorithm>
#include <random>
#include "../instance/InstanceJob.h"
#include "Batch.h"

using namespace std;

struct Batch;

struct Job {

	InstanceJob *instanceJob;
	bool assignedToBatch = false;
	bool assignedToMachine = false;
	Batch *batch = NULL;
	double priority = -1;
	double weightPriority;

	// Redundancy
	int batchIndexPosition = -1;

public:

	Job(InstanceJob *instanceJob) {
		this->instanceJob = instanceJob;
	}

	bool isDummy() {
		return this->instanceJob == NULL;
	}

	string toString() {
		if (this->isDummy()) {
			return "{dummy job}";
		} else {
			string assignedStr = this->assignedToBatch ? "true" : "false";
			return "{instance: " + this->instanceJob->toString() + ", assigned: " + assignedStr + ", priority: " + to_string(this->priority) + "}";
		}
	}
};

inline static Job DUMMY_JOB = Job(NULL);

