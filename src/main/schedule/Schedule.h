#pragma once

#include <vector>
#include <iostream>
#include <float.h>

#include "../instance/Instance.h"
#include "../instance/InstanceJob.h"
#include "../instance/InstanceMachine.h"
#include "../instance/InstanceFamily.h"

#include "Batch.h"
#include "Machine.h"

using namespace std;

struct Job;

struct Schedule {

	inline static int ID_COUNTER = 0;

	int id;
	Instance *instance;
	vector<Machine*> machines;
	vector<Job*> jobs;

	int cost = INT_MAX;
	int totalAssignedJobs = 0;

	// twd seed

	int thres;
	int lookAheadParam;

public:

	Schedule(Instance *instance) {
		this->id = ++ID_COUNTER;
		this->instance = instance;

		// Machines
		this->machines.resize(instance->instanceMachines.size());
		for (int index = 0; index < instance->instanceMachines.size(); index++) {
			machines[index] = new Machine(instance->instanceMachines[index], instance->instanceJobs.size(), this);
		}

		// Jobs
		this->jobs.resize(instance->instanceJobs.size());
		for (int j = 0; j < instance->instanceJobs.size(); j++) {
			this->jobs[j] = new Job(instance->instanceJobs[j]);
		}
	}

	Schedule(Instance *instance, int thres, int lookAheadParam) : Schedule(instance) {
		this->thres = thres;
		this->lookAheadParam = lookAheadParam;
	}

	~Schedule() {
		for (Machine *machine : machines) {
			delete machine;
		}
		for (Job *job : jobs) {
			delete job;
		}
	}

	bool hasUnassignedJobs() {
		return jobs.size() != totalAssignedJobs;
	}

	int firstJobIndex(InstanceFamily *instanceFamily) {
		return instanceFamily->id * instanceFamily->numJobs;
	}

	int lastJobIndex(InstanceFamily *instanceFamily) {
		return firstJobIndex(instanceFamily) + instanceFamily->numJobs - 1;
	}

	bool hasUnassignedJobs(InstanceFamily *instanceFamily) {
		int firstIndex = firstJobIndex(instanceFamily);
		for (int j = firstIndex; j < firstIndex + instanceFamily->numJobs; j++) {
			if (!jobs[j]->assignedToMachine) {
				return true;
			}
		}
		return false;
	}

	vector<Job*> getJobsFromFamily(InstanceFamily *instanceFamily) {
		vector<Job*> jobsFromFamily;
		int firstIndex = firstJobIndex(instanceFamily);
		int lastIndex = lastJobIndex(instanceFamily);
		jobsFromFamily.insert(jobsFromFamily.begin(), jobs.begin() + firstIndex, jobs.begin() + lastIndex + 1);
		return jobsFromFamily;
	}

	int totalBatches() {
		int total = 0;
		for (Machine *machine : machines) {
			total += machine->numBatches;
		}
		return total;
	}
};
