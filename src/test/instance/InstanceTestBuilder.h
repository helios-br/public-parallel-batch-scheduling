#pragma once

#include <iostream>
#include <vector>
#include "../instance/InstanceJobTestBuilder.h"
#include "../instance/InstanceFamilyTestBuilder.h"
#include "../../main/instance/Instance.h"
#include "../../main/instance/InstanceFamily.h"
#include "../../main/instance/InstanceJob.h"
#include "../../main/instance/InstanceMachine.h"

using namespace std;

struct InstanceTestBuilder {

private:
	string instanceName = "";
	int maxBatchSize;
	vector<InstanceFamily*> instanceFamilies;
	vector<InstanceJob*> instanceJobs;
	vector<InstanceMachine*> instanceMachines;

	InstanceTestBuilder(int maxBatchSize) {
		this->maxBatchSize = maxBatchSize;
	}

public:

	inline static InstanceTestBuilder* createInstance(int maxBatchSize) {
		return new InstanceTestBuilder(maxBatchSize);
	}

	InstanceTestBuilder* withMaxBatchSize(int maxBatchSize) {
		this->maxBatchSize = maxBatchSize;
		return this;
	}

	InstanceTestBuilder* withInstanceFamilies(vector<InstanceFamily*> instanceFamilies) {
		this->instanceFamilies = instanceFamilies;
		return this;
	}

	InstanceTestBuilder* addFamily(InstanceFamily *instanceFamily) {
		this->instanceFamilies.push_back(instanceFamily);
		return this;
	}

	InstanceTestBuilder* withInstanceJobs(vector<InstanceJob*> instanceJobs) {
		this->instanceJobs = instanceJobs;
		return this;
	}

	InstanceTestBuilder* addJob(InstanceJob *instanceJob) {
		this->instanceJobs.push_back(instanceJob);
		return this;
	}

	InstanceTestBuilder* withInstanceMachines(vector<InstanceMachine*> instanceMachines) {
		this->instanceMachines = instanceMachines;
		return this;
	}

	Instance build() {
		return Instance(instanceName, instanceJobs, instanceFamilies, instanceMachines, maxBatchSize, 1);
	}

};
