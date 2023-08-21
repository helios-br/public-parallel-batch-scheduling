#pragma once

#include "InstanceJob.h"
#include "InstanceMachine.h"
#include "../util/StringUtil.h"

#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct Instance {

	string instanceName;
	int maxBatchSize;
	// All families
	vector<InstanceFamily*> instanceFamilies;
	// All jobs of all families
	vector<InstanceJob*> instanceJobs;
	// All machines
	vector<InstanceMachine*> instanceMachines;
	// Redundancy
	int numberFamilies;
	int numberOfJobsPerFamily;
	int multiplier;

	Instance(
			string instanceName,
			vector<InstanceJob*> instanceJobs,
			vector<InstanceFamily*> instanceFamilies,
			vector<InstanceMachine*> instanceMachines,
			int maxBatchSize,
			int multiplier) {
		this->instanceName = instanceName;
		this->instanceJobs = instanceJobs;
		this->instanceFamilies = instanceFamilies;
		this->instanceMachines = instanceMachines;
		this->maxBatchSize = maxBatchSize;
		this->numberFamilies = instanceFamilies.size();
		this->numberOfJobsPerFamily = instanceJobs.size() / this->numberFamilies;
		this->multiplier = multiplier;
	}

	void print() {
		cout << endl;
		cout << "---- Instance data ----" << endl;
		cout << "Name: " << this->instanceName << endl;
		cout << "Max batch size: " << this->maxBatchSize << endl;
		cout << "Number of families: " << this->numberFamilies << endl;
		cout << "Total jobs: " << this->instanceJobs.size() << endl;
		cout << "Total machines: " << this->instanceMachines.size() << endl;
		cout << "Total families: " << this->instanceFamilies.size() << endl;
	}

	void printJobs() {
		for (InstanceJob *instanceJob : instanceJobs) {
			instanceJob->print();
		}
	}

	void printAll() {
		print();
		cout << endl;
		printJobs();
	}
};
